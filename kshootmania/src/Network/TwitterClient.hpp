#pragma once
#include <Siv3D.hpp>

namespace Twitter
{
	struct Token
	{
		String token;
		String secret;
	};

	class TwitterClient
	{
	public:
		TwitterClient();

		// Starts the OAuth 1.0a flow (Mock: Opens a URL and returns a fake PIN)
		// In a real app, this would get a Request Token, open the browser, and ask the user for a PIN.
		void signIn();

		// Verifies the PIN entered by the user and swaps it for an Access Token
		Co::Task<bool> verifyPin(StringView pin);

		// Posts a tweet with an optional image
		Co::Task<bool> postTweet(StringView text, const Optional<Image>& image = none);

		[[nodiscard]]
		bool isSignedIn() const;

	private:
		Token m_accessToken;
		bool m_isSignedIn = false;
	};
}
