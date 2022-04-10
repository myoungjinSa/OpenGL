#pragma once
#include "Types.h"
class Drag {
public:
	Drag();

	void Begin(const Point2d& pt);
	void Track(const Point2d& pt);
	void End();

	void SetMinTrackedSize(Size2u size) { minTrackedSize = size; }
	bool IgnoreDrag();
protected:
	Point2d beginPoint;
	Point2d endPoint;
	bool    bTracking;
	bool    bTracked;

	Size2u minTrackedSize;
};

class DragContext {
public:
	DragContext() {
		Init();
	}

	void Init();
	void Update();

	bool bDelayedSelect;
	bool bShiftKeyDown;
	bool bControlKeyDown;
	bool bAltKeyDown;
};