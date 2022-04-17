#include "Drag.h"
#include "System.h"

Drag::Drag() 
	: bTracking(false)
	, minTrackedSize(3, 3)
{

}

void Drag::Begin(const Point2i& point) {
	beginPoint = point;
	endPoint = point;
	bTracking = true;
	bTracked = false;
}

void Drag::Track(const Point2i& point) {
	endPoint = point;
	if (!bTracked) {
		if (bTracking) {
			if (minTrackedSize.width < abs(point.x - beginPoint.x) || minTrackedSize.height < abs(point.y - beginPoint.y)) {
				bTracked = true;
			}
		}
	}
}
void Drag::End() {
	bTracked = false;
	bTracking = false;
}

bool Drag::IgnoreDrag() {
	Point2d offset(abs(endPoint.x - beginPoint.x), abs(endPoint.y - beginPoint.y));
	if (offset.x <= minTrackedSize.width && offset.y <= minTrackedSize.height)
		return true;

	return false;
}


void DragContext::Init() {
	bDelayedSelect = false;
	Update();
}

void DragContext::Update() {
	bShiftKeyDown = IsKeyDown(VK_SHIFT);
	bControlKeyDown = IsKeyDown(VK_CONTROL);
	bAltKeyDown = IsKeyDown(VK_MENU);
}

