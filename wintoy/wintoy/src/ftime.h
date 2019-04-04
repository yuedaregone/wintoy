#ifndef __MTIME_H__
#define __MTIME_H__
#include "wintoy.h"
class Time
{
public:
	static inline float time() { return m_time; }
	static inline float delay() { return m_delay; }
	static inline float scale() { return m_scale; }
	static inline void sscale(float s) { m_scale = s; }

	static inline float rtime() { return m_realTime; }
	static inline float rdelay() { return m_realDelay; }
private:
	static float m_time;
	static float m_delay;
	static float m_scale;

	static float m_realTime;
	static float m_realDelay;

	static float m_freq;

public:
	static inline void UpdateTime(LONGLONG interval)
	{
		m_realDelay = (float)(m_freq * interval);
		m_delay = m_realDelay * m_scale;

		m_realTime += m_realDelay;
		m_time += m_delay;
	}

	static inline void SetFrequency(LONGLONG freq)
	{
		m_freq = (float)(1.0 / freq);
	}
};

#endif
