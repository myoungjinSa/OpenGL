#pragma once
#include "Types.h"
class Drag {
public:
	Drag();

	void Begin(const Point2i& pt);
	void Track(const Point2i& pt);
	void End();

	void SetMinTrackedSize(Size2u size) { minTrackedSize = size; }
	bool IgnoreDrag();

	bool IsTracked()const { return bTracked; }

	Point2i GetBeginPoint()const { return beginPoint; }
	Point2i GetEndPoint()const { return endPoint; }
protected:
	Point2i beginPoint;
	Point2i endPoint;
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