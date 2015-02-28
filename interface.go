package timer

type TimerInterface interface {
	Run()
	Pause()
	Close()
}
