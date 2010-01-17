#include "StdAfx.h"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include "Mind.h"

Mind::Mind(void)
{
}

Mind::~Mind(void)
{
}

void Mind::Run()
{
	do
	{
		if (!ReachGoals())
		{
			BOOST_FOREACH(GoalGenerator::ptr_t generator, goalGenerators_)
			{
				goals_.push_back(
					generator->GenerateGoal() );
			}
		}
	}
	while (!goals_.empty());
}

void Mind::AddGoalGenerator( GoalGenerator::ptr_t generator )
{
	generator->Bind(
		boost::bind(&goals_collection_t::push_back, goals_, _1 ));

	goalGenerators_.insert(generator);
}

bool Mind::ReachGoals()
{
	bool someRiched = false;

	if (goals_.size())
	{
		for( goals_collection_t::iterator it = goals_.begin();
			it != goals_.end(); ++it )
		{
			Goal::ptr_t goal = *it;
			bool isRiched = std::find(richedGoals_.begin(), richedGoals_.end(), goal) != richedGoals_.end();
			someRiched = isRiched || goal->Rich();
			if (someRiched)
			{
				if (!isRiched) // it is riched by now
				{
					richedGoals_.insert(richedGoals_.begin(), it, it);
				}

				goals_.erase( it );
			}
			else
			{
				Goal::container_t newGoals = goal->ToRich();
				goals_.insert( it, newGoals.begin(), newGoals.end() ); 
			}
		}
	}

	return someRiched;
}