#include "animation.h"
#include "transform.h"
#include <glm/gtx/quaternion.hpp>
namespace components::animations {
	RotateAroundForever::RotateAroundForever(
		glm::vec3 axis,
		float speedInDegressPerSecond,
		components::Transform* target) :
		mAxis(axis), mSpeed(speedInDegressPerSecond),
		components::Animation(), mCurrentAngleInDegs(0),
		mTarget(target)
	{
	}

	void RotateAroundForever::Advance(float deltaTime)
	{
		this->mCurrentAngleInDegs += mSpeed * deltaTime;
		glm::quat dest = glm::angleAxis(glm::radians(mCurrentAngleInDegs), mAxis);
		glm::quat finalQuat = glm::lerp(mTarget->mOrientation, dest, 0.5f);
		mTarget->mOrientation.w = finalQuat.w;
		mTarget->mOrientation.x = finalQuat.x;
		mTarget->mOrientation.y = finalQuat.y;
		mTarget->mOrientation.z = finalQuat.z;
	}
	bool RotateAroundForever::IsFinished() const
	{
		return false;
	}
	Move::Move(glm::vec3 direction, 
		float speed, 
		float time,
		on_end_t onEnd, 
		Transform* target)
		:mDirection(glm::normalize(direction)), mSpeed(speed), mTime(time), mOnEnd(onEnd), mCurrentTime(0), mFinished(false),
		mTarget(target)
	{
	}
	void Move::Advance(float deltaTime)
	{
		mCurrentTime += mSpeed * deltaTime;
		if (mCurrentTime >= mTime)
			mFinished = true;
		else {
			glm::vec3 currentPos = mTarget->mPosition;
			currentPos += mDirection * deltaTime;
			mTarget->mPosition = currentPos;
		}

	}
	bool Move::IsFinished() const
	{
		return mFinished;
	}
	void Move::OnEnd()
	{
		mOnEnd();
	}
}