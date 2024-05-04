#include "Node.hh"
#include "RigidTransform.hxx"
#include "engine/algebra/Quaternion.hxx"
#include "engine/timeline/Anim.hxx"

using namespace Core;

void Node::Update(int objectTime, int oldObjectTime)
{
	RigidTransform transform = localTransform;
	RigidTransform oldTransform = localTransform;

	if (animation.delegate != NULL || animation.Duration() > 0)
	{
		transform *= animation.Get(objectTime);
		oldTransform *= animation.Get(oldObjectTime);
	}

	if (parent != NULL)
	{
		transform = parent->globalTransform * transform;
		oldTransform = parent->oldGlobalTransform * oldTransform;
	}

	globalTransform = transform;
	oldGlobalTransform = oldTransform;
}
