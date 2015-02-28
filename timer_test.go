package timer

import (
	"testing"
	"time"
)

// Test for Timer func
func Test_Timer(t *testing.T) {
	timer, err := NewTimer(REAL_TIME, "timer")
	defer time.Sleep(100 * time.Millisecond)
	if err != nil {
		t.Error("Fail to create `Timer`!")
	} else if timer != nil {
		defer timer.Close()
	}
	go func() {
		count := 0
		runEvent := 0
		pauseEvent := 0
		closeEvent := 0
		for {
			select {
			case tt := <-timer.C:
				count++
				t.Log(count, tt)
			case <-timer.RunEvent:
				runEvent++
				t.Log("run")
			case <-timer.PauseEvent:
				pauseEvent++
				t.Log("pause")
			case <-timer.CloseEvent:
				closeEvent++
				t.Log("close")
				//
				if count > 10 && runEvent > 0 && pauseEvent > 0 && closeEvent > 0 {
					t.Log("Succcess!")
				} else {
					t.Error("Fail!")
				}
				break
			}
		}
	}()
	begin := time.Date(2014, time.April, 1, 0, 0, 0, 0, time.UTC)
	duration := 10 * time.Millisecond
	timer.Run(begin, duration, 0)
	time.Sleep(50 * time.Millisecond)
	timer.Pause()
	time.Sleep(50 * time.Millisecond)
	timer.Run(begin, duration, 0)
	time.Sleep(50 * time.Millisecond)
}
