#pragma once
#include "Event.h"

namespace dae
{
	class GameObject;

	class IObserver
	{
	public:
		virtual void Notify(const Event& event, GameObject* actor) = 0;
	};
}