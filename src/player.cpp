#include "player.h"

void Player::begin() {}

void Player::play() {}

void Player::pause() {}

void Player::togglePlayPause() {}

void Player::nextTrack() {}

void Player::previousTrack() {}

void Player::nextFolder() {}

void Player::previousFolder() {}

void Player::volumeUp() {}

void Player::volumeDown() {}

void Player::setVolume(int volume)
{
    volumeLevel = volume;
}

bool Player::isPlaying() const
{
    return false;
}

int Player::currentTrack() const
{
    return 0;
}

int Player::currentFolder() const
{
    return 0;
}

int Player::volume() const
{
    return volumeLevel;
}