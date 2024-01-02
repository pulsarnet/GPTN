use std::sync::{Arc, Condvar, Mutex, Weak};

pub struct Semaphore(Arc<SemaphoreInner>);

struct SemaphoreInner {
    counter: Mutex<i32>,
    condvar: Condvar,
}

impl Semaphore {
    pub fn new(count: i32) -> Self {
        Semaphore(Arc::new(SemaphoreInner {
            counter: Mutex::new(count),
            condvar: Condvar::new(),
        }))
    }

    pub fn acquire(&self) -> SemaphoreGuard {
        let mut count = self.0.counter.lock().unwrap();
        while *count <= 0 {
            count = self.0.condvar.wait(count).unwrap();
        }
        *count -= 1;
        SemaphoreGuard(Arc::downgrade(&self.0))
    }
}

pub struct SemaphoreGuard(Weak<SemaphoreInner>);

impl Drop for SemaphoreGuard {
    fn drop(&mut self) {
        if let Some(semaphore) = self.0.upgrade() {
            let mut count = semaphore.counter.lock().unwrap();
            *count += 1;
            semaphore.condvar.notify_one();
        }
    }
}