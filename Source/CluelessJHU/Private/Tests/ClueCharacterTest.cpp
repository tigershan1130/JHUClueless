#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "CluelessJHU/Actors/ClueCharacter.h"

static const int TestFlags = (
	EAutomationTestFlags::CommandletContext
	| EAutomationTestFlags::ClientContext
	| EAutomationTestFlags::ProductFilter);


// Simplest test you can run
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAClueCharacterTest, "Clueless.Gameplay.Basic", TestFlags)
bool FAClueCharacterTest::RunTest(const FString& Parameters)
{
	TestEqual("Testing that 2+2 == 4", 2 + 2, 4);
	return true;
}