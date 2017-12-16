Roles
--

The Third-Party Application: "Client"

The client is the application that is attempting to get access to the user's account. It needs to get permission from the user before it can do so.
The API: "Resource Server"

The resource server is the API server used to access the user's information.
The User: "Resource Owner"

The resource owner is the person who is giving access to some portion of their account.
Creating an App

Before you can begin the OAuth process, you must first register a new app with the service. When registering a new app, you usually register basic information such as application name, website, a logo, etc. In addition, you must register a redirect URI to be used for redirecting users to for web server, browser-based, or mobile apps.


Redirect URIs
--

The service will only redirect users to a registered URI, which helps prevent some attacks. Any HTTP redirect URIs must be protected with TLS security, so the service will only redirect to URIs beginning with "https". This prevents tokens from being intercepted during the authorization process.
Client ID and Secret

After registering your app, you will receive a client ID and a client secret. The client ID is considered public information, and is used to build login URLs, or included in Javascript source code on a page. The client secret must be kept confidential. If a deployed app cannot keep the secret confidential, such as Javascript or native apps, then the secret is not used.
Authorization

The first step of OAuth 2 is to get authorization from the user. For browser-based or mobile apps, this is usually accomplished by displaying an interface provided by the service to the user.

OAuth 2 provides several "grant types" for different use cases. The grant types defined are:

    Authorization Code for apps running on a web server
    Implicit for browser-based or mobile apps
    Password for logging in with a username and password
    Client credentials for application access

Each use case is described in detail below.
Web Server Apps

Web server apps are the most common type of application you encounter when dealing with OAuth servers. Web apps are written in a server-side language and run on a server where the source code of the application is not available to the public.


Authorization
--

Create a "Log In" link sending the user to:

https://oauth2server.com/auth?response_type=code&
  client_id=CLIENT_ID&redirect_uri=REDIRECT_URI&scope=photos

The user sees the authorization prompt

OAuth Authorization Prompt

If the user clicks "Allow," the service redirects the user back to your site with an auth code

https://oauth2client.com/cb?code=AUTH_CODE_HERE

Your server exchanges the auth code for an access token

POST https://api.oauth2server.com/token
    grant_type=authorization_code&
    code=AUTH_CODE_HERE&
    redirect_uri=REDIRECT_URI&
    client_id=CLIENT_ID&
    client_secret=CLIENT_SECRET

The server replies with an access token

{
    "access_token":"RsT5OjbzRn430zqMLgV3Ia"
}

or if there was an error

{
    "error":"invalid_request"
}

Security: Note that the service must require apps to pre-register their redirect URIs.
Browser-Based Apps

Browser-based apps run entirely in the browser after loading the source code from a web page. Since the entire source code is available to the browser, they cannot maintain the confidentiality of their client secret, so the secret is not used in this case.
Authorization

Create a "Log In" link sending the user to:

https://oauth2server.com/auth?response_type=token&
  client_id=CLIENT_ID&redirect_uri=REDIRECT_URI&scope=photos

The user sees the authorization prompt


OAuth Authorization Prompt
--

If the user clicks "Allow," the service redirects the user back to your site with an access token

https://oauth2client.com/cb#token=ACCESS_TOKEN

That's it, there's no other steps! At this point, some Javascript code can pull out the access token from the fragment (the part after the #) and begin making API requests.

If there was an error, you will instead receive an error code in the URI fragment, such as:

https://oauth2client.com/cb#error=access_denied

Mobile Apps

Like browser-based apps, mobile apps also cannot maintain the confidentiality of their client secret. Because of this, mobile apps must also use an OAuth flow that does not require a client secret.
Authorization

Create a "Log in" button sending the user to either the native app of the service on the phone, or a mobile web page for the service. On iPhone, apps can register a custom URI protocol such as "facebook://" so the native Facebook app is launched whenever a URL with that protocol is visited. On Android, apps can register URL matching patterns which will launch the native app if a URL matching the pattern is visited.
iPhone

If the user has the native Facebook app installed, direct them to the following URL:

fbauth2://authorize?response_type=token&client_id=CLIENT_ID
  &redirect_uri=REDIRECT_URI&scope=email

In this case, your redirect URI looks like fb00000000://authorize where the protocol is "fb" followed by your app's client ID. This means your app must be registered to open URLs with that protocol.
Android or Others

If the user does not have the Facebook iPhone app, or for other devices, you can launch a mobile browser to the standard web authorization URL.

https://facebook.com/dialog/oauth?response_type=token
  &client_id=CLIENT_ID&redirect_uri=REDIRECT_URI&scope=email

The user will see the authorization prompt


Facebook Authorization Prompt
--

After clicking "Okay", the user will be redirected back to your application by a URL like

fb00000000://authorize#token=ACCESS_TOKEN

Your mobile application can parse out the access token from the URI and begin using it to make API requests.
Others
Password

OAuth 2 also provides a "password" grant type which can be used to exchange a username and password for an access token directly. Since this obviously requires the application to collect the user's password, it should only be used by apps created by the service itself. For example, the native Twitter app could use this grant type to log in on mobile or desktop apps.

To use the password grant type, simply make a POST request like the following:

POST https://api.oauth2server.com/token
    grant_type=password&
    username=USERNAME&
    password=PASSWORD&
    client_id=CLIENT_ID

The server replies with an access token in the same format as the other grant types.

Note, the client secret is not included here under the assumption that most of the use cases for password grants will be mobile or desktop apps, where the secret cannot be protected.
Application access

In some cases, applications may wish to update their own information such as their website URL or application icon, or they may wish to get statistics about the users of the app. In this case, applications need a way to get an access token for their own account, outside the context of any specific user. OAuth provides the client_credentials grant type for this purpose.

To use the client credentials grant type, make a POST request like the following:

POST https://api.oauth2server.com/token
    grant_type=client_credentials&
    client_id=CLIENT_ID&
    client_secret=CLIENT_SECRET

You will get an access token response in the same format as the other grant types.
Making Authenticated Requests

Now that you have an access token, you can make requests to the API. You can quickly make an API request using cURL as follows:

curl -H "Authorization: Bearer RsT5OjbzRn430zqMLgV3Ia" \
https://api.oauth2server.com/1/me

That's it! Make sure you always send requests over HTTPS and never ignore invalid certificates. HTTPS is the only thing protecting requests from being intercepted or modified.
Differences from OAuth 1.0

OAuth 1.0 was largely based on existing proprietary protocols such as Flickr's "FlickrAuth" and Google's "AuthSub". The result represented the best solution based on actual implementation experience. However, after several years of working with the protocol, the community learned enough to rethink and improve the protocol in three main areas where OAuth 1.0 proved limited or confusing:
Authentication and Signatures

The majority of developers' confusion and annoyance with OAuth 1.0 was due to the cryptographic requirements of signing requests with the client ID and secret. Losing the ability to easily copy and paste cURL examples made it much more difficult to get started quickly.

OAuth 2 recognizes this difficulty and replaces signatures with requiring HTTPS for all communications between browsers, clients and the API.
User Experience and Alternative Authorization Flows

OAuth includes two main parts, obtaining an access token, and using the access token to make requests. OAuth 1.0 works best for desktop web browsers, but fails to provide a good user experience for native desktop and mobile apps or alternative devices such as game or TV consoles.

OAuth 2 supports a better user experience for native applications, and supports extending the protocol to provide compatibility with future device requirements.
Performance at Scale

As larger providers started to use OAuth 1.0, the community quickly realized the protocol does not scale well. Many steps require state management and temporary credentials, which require shared storage and are difficult to synchronize across data centers. OAuth 1.0 also requires that the API server has access to the application's ID and secret, which often breaks the architecture of most large providers where the authorization server and API servers are completely separate.

OAuth 2 supports the separation of the roles of obtaining user authorization and handling API calls. Larger providers needing this scalability are free to implement it as such, and smaller providers can use the same server for both roles if they wish.
