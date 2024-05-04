#ifndef NODE_HH
#define NODE_HH

#include "RigidTransform.hh"
#include "engine/timeline/Anim.hh"

namespace Core
{
	struct Node
	{
		const Node*		parent;

		RigidTransform	localTransform;
		RigidTransform	globalTransform;
		RigidTransform	oldGlobalTransform;

		int				startDate;
		int				endDate;

		Timeline::Anim<RigidTransform>	animation;

		void Update(int objectTime, int oldObjectTime);
	};
}

#endif // NODE_HH
