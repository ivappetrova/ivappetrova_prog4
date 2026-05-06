#pragma once
#include "Event.h"

namespace dae
{
	class GameObject;

	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void Notify(const Event& event, GameObject* actor) = 0;

		IObserver() = default;
		IObserver(const IObserver&) = delete;
		IObserver(IObserver&&) = delete;
		IObserver& operator=(const IObserver&) = delete;
		IObserver& operator=(IObserver&&) = delete;
	};
}