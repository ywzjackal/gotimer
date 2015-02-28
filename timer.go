package timer

//#cgo LDFLAGS: -lrt
//#include "timer.h"
import "C"
import (
	"time"
	"unsafe"
)

const (
	REAL_TIME = 0
	MONO_TIME = 1
)

type Timer struct {
	// interface
	TimerInterface
	// public
	Name       string //used for debug
	C          chan time.Time
	PauseEvent chan time.Time
	RunEvent   chan time.Time
	CloseEvent chan time.Time
	// private
	jktimer C.struct_jktimer_t
}

//export GoCallback
func GoCallback(p unsafe.Pointer) {
	var t = (*Timer)(p)
	//	log.Println("Timer arrived", t.Name, time.Now())
	t.C <- time.Now()
}

func NewTimer(clockType int, name string) (t *Timer, err error) {
	//
	t = &Timer{
		Name:       name,
		C:          make(chan time.Time, 1),
		RunEvent:   make(chan time.Time, 1),
		PauseEvent: make(chan time.Time, 1),
		CloseEvent: make(chan time.Time, 1),
	}
	//
	pn := C.CString(name)
	defer C.free(unsafe.Pointer(pn))
	jkt := (*C.struct_jktimer_t)(unsafe.Pointer(&t.jktimer))
	ct := (C.clockid_t)(clockType)
	cp := unsafe.Pointer(t)
	C.jktimer_init(jkt, ct, 10, cp, pn)
	return
}

func (t *Timer) Run(begin time.Time, interval time.Duration, count int) (err error) {
	var (
		sec   C.longlong
		nsec  C.int
		bsec  C.longlong
		bnsec C.int
	)
	sec = (C.longlong)(interval.Seconds())
	nsec = (C.int)(interval.Nanoseconds() % (int64)(time.Second))
	bsec = (C.longlong)(begin.Unix())
	bnsec = (C.int)(begin.Nanosecond())

	jkt := (*C.struct_jktimer_t)(unsafe.Pointer(&t.jktimer))
	cc := (C.int)(count)

	C.jktimer_start(jkt, bsec, bnsec, sec, nsec, cc)
	t.RunEvent <- time.Now()
	return
}

func (t *Timer) Pause() (err error) {
	jkt := (*C.struct_jktimer_t)(unsafe.Pointer(&t.jktimer))
	C.jktimer_stop(jkt)
	t.PauseEvent <- time.Now()
	return
}

func (t *Timer) Close() (err error) {
	jkt := (*C.struct_jktimer_t)(unsafe.Pointer(&t.jktimer))
	C.jktimer_delete(jkt)
	t.CloseEvent <- time.Now()
	return
}
