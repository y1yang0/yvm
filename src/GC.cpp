#include "Concurrent.hpp"
#include "GC.h"
#include "JavaClass.h"
#include "JavaHeap.h"
#include "JavaType.h"
#include "MethodArea.h"
#include "YVM.h"
#include <atomic>

void ConcurrentGC::stopTheWorld() {
	unique_lock<mutex> lock(safepointWaitMtx);
	safepointWaitCnt++;
	while (safepointWaitCnt != YVM::executor.getThreadNum()) {
		safepointWaitCond.wait(lock);
	}
	safepointWaitCond.notify_all();
}

void ConcurrentGC::GCThreadPool::finalize() {
	work = true;
	done = true;
	sleepCnd.notify_all();
}

void ConcurrentGC::GCThreadPool::runPendingWork() {
	while (!done) {
		unique_lock<mutex> lock(sleepMtx);
		while (work != true) {
			sleepCnd.wait(lock);
		}

		taskQueueMtx.lock();
		if (!taskQueue.empty()) {
			auto task = std::move(taskQueue.front());
			taskQueue.pop();
			taskQueueMtx.unlock();
			task();
		} else {
			taskQueueMtx.unlock();
		}
	}
}

void ConcurrentGC::gc(GCPolicy policy) {
	lock_guard<mutex> lock(overMemoryThresholdMtx);
	if (!overMemoryThreshold) {
		return;
	}

	switch (policy) {
	case GCPolicy::GC_MARK_AND_SWEEP:
		gcThreadPool.signalWork();
		markAndSweep();
		break;
	default:
		gcThreadPool.signalWork();
		markAndSweep();
		break;
	}
	objectBitmap.clear();
	arrayBitmap.clear();
	overMemoryThreshold = false;
	gcThreadPool.signalWait();
}

void ConcurrentGC::mark(JType* ref) {
	if (ref == nullptr) {
		// Prevent from throwing std::bad_typeid since local variable table slot could be empty
		return;
	}
	if (typeid(*ref) == typeid(JObject)) {
		{
			// Mark() is very quickly and busy, so we use lightweight spin lock instead of stl mutex
			std::lock_guard<SpinLock> lock(objSpin);
			objectBitmap.insert(dynamic_cast<JObject*>(ref)->offset);
		}
		auto fields =
		    yrt.jheap->getObjectFieldsByOffset(dynamic_cast<JObject*>(ref)->offset);
		for (size_t i = 0; i < fields.size(); i++) {
			mark(fields[i]);
		}
	} else if (typeid(*ref) == typeid(JArray)) {
		{
			std::lock_guard<SpinLock> lock(arrSpin);
			arrayBitmap.insert(dynamic_cast<JArray*>(ref)->offset);
		}
		auto items = yrt.jheap->getArrayItemsByOffset(dynamic_cast<JArray*>(ref)->offset);

		for (size_t i = 0; i < items.first; i++) {
			mark(items.second[i]);
		}
	} else {
		SHOULD_NOT_REACH_HERE
	}
}

void ConcurrentGC::sweep() {
	future<void> objectFuture = gcThreadPool.submit([this]() -> void {
		for (auto pos = yrt.jheap->objheap.begin(); pos != yrt.jheap->objheap.end();) {
			// If we can not find active object in object bitmap then clear it
			// Notice that here we don't need to lock objectBitmap since it must
			// be marked before sweeping
			if (objectBitmap.find(pos->first) == objectBitmap.cend()) {
				yrt.jheap->objheap.erase(pos++);
			} else {
				++pos;
			}
		}
	});

	future<void> arrayFuture = gcThreadPool.submit([this]() -> void {
		for (auto pos = yrt.jheap->arrheap.begin(); pos != yrt.jheap->arrheap.end();) {
			// DITTO
			if (arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
				for (size_t i = 0; i < pos->second.first; i++) {
					delete pos->second.second[i];
				}
				delete[] pos->second.second;
				yrt.jheap->arrheap.erase(pos++);
			} else {
				++pos;
			}
		}
	});

	future<void> monitorFuture = gcThreadPool.submit([this]() -> void {
		// DITTO
		for (auto pos = yrt.jheap->monitorheap.begin();
		     pos != yrt.jheap->monitorheap.end();) {
			if (objectBitmap.find(pos->first) == objectBitmap.cend() ||
			    arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
				yrt.jheap->monitorheap.erase(pos++);
			} else {
				++pos;
			}
		}
	});

	objectFuture.get();
	arrayFuture.get();
	monitorFuture.get();
}

void ConcurrentGC::markAndSweep() {
	vector<future<void>> stackMarkFuture, localMarkFuture;
	for (auto frame = frames.cbegin(); frame != frames.cend(); ++frame) {
		stackMarkFuture.push_back(gcThreadPool.submit([this, frame]() -> void {
			for (auto stackSlot = (*frame)->stack.cbegin();
			     stackSlot != (*frame)->stack.cend(); ++stackSlot) {
				this->mark(*stackSlot);
			}
		}));

		localMarkFuture.push_back(gcThreadPool.submit([this, frame]() -> void {
			for (auto localSlot = (*frame)->locals.cbegin();
			     localSlot != (*frame)->locals.cend(); ++localSlot) {
				this->mark(*localSlot);
			}
		}));
	}

	future<void> staticFieldsFuture = gcThreadPool.submit([this]() -> void {
		for (auto c : yrt.ma->classTable) {
			std::for_each(c.second->sfield.cbegin(), c.second->sfield.cend(), [this](const pair<size_t, JType*>& offset) {
				if (typeid(*offset.second) == typeid(JObject)) {
					{
						std::lock_guard<SpinLock> lock(objSpin);
						objectBitmap.insert(offset.first);
					}
				} else if (typeid(*offset.second) == typeid(JArray)) {
					{
						std::lock_guard<SpinLock> lock(arrSpin);
						arrayBitmap.insert(offset.first);
					}
				}
			});
		}
	});

	staticFieldsFuture.get();
	if (!frames.empty()) {
		for (auto& sk : stackMarkFuture) {
			sk.get();
		}
		for (auto& lv : localMarkFuture) {
			lv.get();
		}
	}
	sweep();
}
