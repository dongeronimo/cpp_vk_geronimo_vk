#pragma once
#include <glm/glm.hpp>
#include <functional>
#include <optional>
namespace components {
	class Transform;
	class Animation {
	public:
		virtual ~Animation() = default;
		virtual void Advance(float deltaTime) = 0;
		virtual bool IsFinished()const = 0;
		virtual void OnStart() {}
		virtual void OnEnd() {}
		virtual void Reset() {}
	};

	namespace animations {
		/// <summary>
		/// Rotate a Transform around an axis. This rotation will never stop.
		/// </summary>
		class RotateAroundForever :public Animation {
		public:
			RotateAroundForever(
				glm::vec3 axis,
				float speedInDegressPerSecond,
				Transform* target
			);
			void Advance(float deltaTime) override;
			bool IsFinished() const override;
		private:
			float mCurrentAngleInDegs;
			const glm::vec3 mAxis;
			const float mSpeed;
			Transform* mTarget;
		};

		class Move : public Animation{
		public:
			typedef std::function<void()> on_end_t;
			Move(glm::vec3 direction, float speed, float time, 
				on_end_t onEnd, Transform* target);
			// Inherited via Animation
			void Advance(float deltaTime) override;
			bool IsFinished() const override;
			void OnEnd()override;

		private:
			bool mFinished;
			Transform* mTarget;
			const float mSpeed;
			const glm::vec3 mDirection;
			const float mTime;
			float mCurrentTime;
			on_end_t mOnEnd;

		};
	}
}