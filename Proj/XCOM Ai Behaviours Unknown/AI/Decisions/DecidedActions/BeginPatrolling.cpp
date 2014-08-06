#include "BeginPatrolling.h"

#include "../../DecisionTree.h"
#include "../../Team.h"
#include "../../AICharacter.h"
#include "../../Actions.h"

//Organises a character with a partner to bounding overwatch, or just patrolo by themselves
//the min comradery needed to consider teammates for bounding overwatch
BeginPatrolling::BeginPatrolling(DecisionTree * tree, int minComraderyForBound)
	: DecisionAction(tree) {

	_minComraderyForBound = minComraderyForBound;
}

//Character should either find team mates to begin bounding overwatch to, or just normally patrol themselves
Action* BeginPatrolling::Run() {

	vector<AICharacter*> willingTeammates = Tree->CharTeam->GetTeammatesOf(Tree->Character);

	//remove any team mates that aren't willing to bounding overwatch
	for (int i = 0; i < (int)willingTeammates.size(); i++) {

		//remove memebers who are not willing to cooperate..
		if (willingTeammates[i]->Comradery < _minComraderyForBound) {

			willingTeammates.erase(willingTeammates.begin() + i);
			i--;
			continue;
		}
	}

	//if this character has high enough comradary and we have willing team mates, look for a bounding partner
	if (Tree->Character->Comradery >= _minComraderyForBound && willingTeammates.size() > 0) {

		//remove any team mates that aren't willing to bounding overwatch
		for (int i = 0; i < (int)willingTeammates.size(); i++) {

			// or are already bounding with someone
			if (willingTeammates[i]->HasAction == true) {

				if (BoundingOverwatch * casted = dynamic_cast<BoundingOverwatch*>(willingTeammates[i]->CurrentAct->FrontAction())) {

					if (casted->ActingCharacter != Tree->Character && casted->CoveringCharacter != Tree->Character) {

						willingTeammates.erase(willingTeammates.begin() + i);
						i--;
						continue;
					}
				}
			}
		}

		//if all teammates are already bounding, then do a normal patrol
		if (willingTeammates.size() == 0) {

			//Tree->Log("No willing teammates to patrol with");
			return new Patrol(Tree->Character, 1);
		}
		
		//loop through all the willing team mates and find ones who are idle
		for (int i = 0; i < (int)willingTeammates.size(); i++) {

			//only link up with teammates that are waiting and only if we are waiting
			if (willingTeammates[i]->HasAction == true && Tree->Character->HasAction == true) {

				if (dynamic_cast<WaitAction*>(willingTeammates[i]->CurrentAct->FrontAction())
					&& willingTeammates[i]->CurrentAct->FrontAction()->IsActionComplete() == false
					&& dynamic_cast<WaitAction*>(Tree->Character->CurrentAct->FrontAction())) {

					//cancel the waiting character's action
					willingTeammates[i]->CurrentAct->HardCancel();
					delete willingTeammates[i]->CurrentAct;
					willingTeammates[i]->HasAction = false;

					return CreateBoundWithTeammate(willingTeammates[i]);
				}
			}
		}

		//if we have made it this far, no willing team mates can do it right now, so just wait
		Tree->Log("Bounding overwatch");
		return new WaitAction(Tree->Character, 1);
	}

	//Tree->Log("No willing teammates to patrol with");
	return new Patrol(Tree->Character, 1);
}

//Creates a bounding overwatch with the provided character, setting its action to the shared overwatch action
Action* BeginPatrolling::CreateBoundWithTeammate(AICharacter * teammate) {

	//bounding overwatch with them
	BoundingOverwatch * newOverwatch = new BoundingOverwatch(teammate, Tree->Character, 2);

	teammate->SetAction(newOverwatch);

	Tree->Log("Bounding overwatch with teammate");
	return newOverwatch;
}
	