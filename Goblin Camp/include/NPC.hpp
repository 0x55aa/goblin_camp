#pragma once

#include <queue>

#include <boost/thread/thread.hpp>
#include <boost/multi_array.hpp>
#include <boost/function.hpp>

#include <libtcod.hpp>

#include "Coordinate.hpp"
#include "Job.hpp"
#include "Entity.hpp"
#include "Container.hpp"
#include "StatusEffect.hpp"
#include "Squad.hpp"

#define LOS_DISTANCE 12
#define MAXIMUM_JOB_ATTEMPTS 5

#define THIRST_THRESHOLD (UPDATES_PER_SECOND * 60 * 5)
#define HUNGER_THRESHOLD UPDATES_PER_SECOND * 60 * 8
#define DRINKABLE_WATER_DEPTH 2
#define WALKABLE_WATER_DEPTH 1

typedef int NPCType;

enum AiThink {
	AINOTHING,
	AIMOVE
};

enum Skill {
	MASONRY,
	CARPENTTY,
	SKILLAMOUNT
};

class SkillSet {
	private:
		int skills[SKILLAMOUNT];
	public:
		SkillSet();
		int operator()(Skill);
		void operator()(Skill, int);
};

class NPC : public Entity {
    friend class Game;

	private:
		NPC(Coordinate,
            boost::function<bool(boost::shared_ptr<NPC>)> findJob,
            boost::function<void(boost::shared_ptr<NPC>)> react);
		NPCType type;
		bool _visArray[LOS_DISTANCE*2 * LOS_DISTANCE*2];
		int timeCount;
		std::deque<boost::shared_ptr<Job> > jobs;
		int taskIndex;
		boost::try_mutex pathMutex;
		TCODPath *path;
		bool nopath;
		bool findPathWorking;
		int timer;
		unsigned int _speed, nextMove;
		bool run;
		TCODColor _color, _bgcolor;
		int _graphic;
		bool taskBegun;
		bool expert;
		boost::weak_ptr<Item> carried;
		int thirst, hunger;
		int thinkSpeed;
		std::list<StatusEffect> statusEffects;
		std::list<StatusEffect>::iterator statusEffectIterator;
		int statusGraphicCounter;
		void HandleThirst();
		void HandleHunger();
		int health;
		boost::weak_ptr<Item> foundItem;
        boost::shared_ptr<Container> bag;

        std::list<boost::weak_ptr<NPC> >nearNpcs;
		bool needsNutrition;

		boost::function<bool(boost::shared_ptr<NPC>)> FindJob;
		boost::function<void(boost::shared_ptr<NPC>)> React;

		int baseStats[STAT_COUNT];
		int effectiveStats[STAT_COUNT];
		bool aggressive;
		boost::weak_ptr<NPC> aggressor;
		bool dead;
		boost::weak_ptr<Squad> squad;

	public:
		~NPC();
		SkillSet Skills;
		AiThink Think();
		void Update();
		void Draw(Coordinate, TCODConsole*);
		void Position(Coordinate,bool);
		virtual void Position(Coordinate);
		virtual Coordinate Position();
		void speed(unsigned int);
		unsigned int speed();
		void color(TCODColor,TCODColor=TCODColor::black);
		void graphic(int);

		bool *visArray();

		Task* currentTask();
		boost::weak_ptr<Job> currentJob();
		Coordinate currentTarget();
		boost::weak_ptr<Entity> currentEntity();
		void TaskFinished(TaskResult, std::string = "");
		TaskResult Move();
		void findPath(Coordinate);

		void AddEffect(StatusEffectType);
		void RemoveEffect(StatusEffectType);
		bool HasEffect(StatusEffectType);
		std::list<StatusEffect>* StatusEffects();

		bool Expert();
		void Expert(bool);

		bool Dead();
		void Kill();
		void DropCarriedItem();
		void Hit(boost::weak_ptr<Entity>);
		void MemberOf(boost::weak_ptr<Squad>);
		boost::weak_ptr<Squad> MemberOf();

		static bool GetSquadJob(boost::shared_ptr<NPC>);
		static bool JobManagerFinder(boost::shared_ptr<NPC>);
		static void PlayerNPCReact(boost::shared_ptr<NPC>);
		static void PeacefulAnimalReact(boost::shared_ptr<NPC>);
		static bool PeacefulAnimalFindJob(boost::shared_ptr<NPC>);
		static void HostileAnimalReact(boost::shared_ptr<NPC>);
		static bool HostileAnimalFindJob(boost::shared_ptr<NPC>);
};

void tFindPath(TCODPath*, int, int, int, int, boost::try_mutex*, bool*, bool*);
