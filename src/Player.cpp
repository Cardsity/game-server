#include "Game.h"
#include "Utils.h"
#include <cpr/cpr.h>

bool Player::ownsCards(std::vector<std::string> cards)
{
	for (auto card : cards)
	{
		if (!LContains<WhiteCard>(hand, card))
			return false;
	}
	return true;
}
bool Player::ownsCard(std::string card)
{
	return LContains<WhiteCard>(hand, card);
}
void Player::playCard(std::string text)
{
	auto card = getItemByValue<WhiteCard>(hand, text);
	if (card.has_value())
	{
		playedCards.push_back(**card);
		hand.erase(*card);
	}
}
void Player::addCard(std::string text)
{
	hand.push_back(WhiteCard{ text, this->owner.id });
}
void Player::autoPlay(uint amount)
{
	for (int i = 0; amount > i; i++)
	{
		auto randomCard = getRandomFromList<WhiteCard>(hand);
		if (randomCard.has_value())
			playCard(randomCard->text);
	}
}