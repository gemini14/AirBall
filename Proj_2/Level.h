#ifndef LEVEL_H
#define LEVEL_H


#include <boost/noncopyable.hpp>
#include <memory>

#include "PhysicsManager.h"


namespace Tuatara
{
	class PhysicsManager;

	class Level : boost::noncopyable
	{
	private:

		

	public:
		std::shared_ptr<PhysicsManager> physics;
		Level();
		~Level();
	};

}

#endif