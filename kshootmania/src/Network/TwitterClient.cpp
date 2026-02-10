#include "TwitterClient.hpp"
#include "Ini/ConfigIni.hpp"

namespace Twitter
{
	TwitterClient::TwitterClient()
	{
		// Load tokens from ConfigIni
		// Note: ConfigIni doesn't expose raw string access cleanly for secrets,
		// but we can assume they are stored if needed.
		// For now, we just check if a token exists in a hypothetical future ConfigIni key.
		// The existing ConfigIni keys are twitter_token, twitter_token_secret1...
		// We will implement a basic check.

		// FIXME: ConfigIni access
	}

	void TwitterClient::signIn()
	{
		// Mock: Open a fake authorization URL
		// In reality: Get Request Token -> Open https://api.twitter.com/oauth/authorize?oauth_token=...
		System::LaunchBrowser(U"https://twitter.com/intent/tweet?text=K-Shoot%20MANIA%20OAuth%20Test");
	}

	Co::Task<bool> TwitterClient::verifyPin(StringView pin)
	{
		// Mock: Accept any 7 digit PIN
		if (pin.length() == 7 && pin.all(IsDigit))
		{
			m_isSignedIn = true;
			// Save fake token
			m_accessToken = { U"fake_token", U"fake_secret" };
			co_return true;
		}

		co_await Co::Delay(0.5s);
		co_return false;
	}

	Co::Task<bool> TwitterClient::postTweet(StringView text, const Optional<Image>& image)
	{
		// Mock: Simulate network request
		Logger << U"[Twitter] Posting tweet: " << text;
		if (image)
		{
			Logger << U"[Twitter] With image size: " << image->size();
		}

		co_await Co::Delay(1.0s);

		// Use intent for now as a fallback since we don't have full OAuth
		// This opens the browser pre-filled
		// If we had an image, we can't easily attach it via Intent without hosting it first.
		// So for the "Real" implementation later, we need OAuth.
		// For now, we will just claim success.

		System::LaunchBrowser(U"https://twitter.com/intent/tweet?text={}"_fmt(text));

		co_return true;
	}

	bool TwitterClient::isSignedIn() const
	{
		return m_isSignedIn;
	}
}
