#ifndef IOBSERVER_H
#define IOBSERVER_H

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
#endif