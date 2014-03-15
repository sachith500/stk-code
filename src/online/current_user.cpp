//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2013 Glenn De Jonghe
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


#include "online/current_user.hpp"

#include "achievements/achievements_manager.hpp"
#include "addons/addon.hpp"
#include "addons/addons_manager.hpp"
#include "config/player_manager.hpp"
#include "config/user_config.hpp"
#include "guiengine/dialog_queue.hpp"
#include "guiengine/screen.hpp"
#include "online/online_profile.hpp"
#include "online/profile_manager.hpp"
#include "online/servers_manager.hpp"
#include "states_screens/login_screen.hpp"
#include "states_screens/online_profile_friends.hpp"
#include "states_screens/dialogs/change_password_dialog.hpp"
#include "states_screens/dialogs/user_info_dialog.hpp"
#include "states_screens/dialogs/notification_dialog.hpp"
#include "utils/log.hpp"
#include "utils/translation.hpp"

#include <sstream>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>

using namespace irr;
using namespace Online;

namespace Online
{
    static CurrentUser* current_user_singleton(NULL);

    /** Singleton create function. */
    CurrentUser* CurrentUser::get()
    {
        if (current_user_singleton == NULL)
            current_user_singleton = new CurrentUser();
        return current_user_singleton;
    }   // get

    // ------------------------------------------------------------------------
    void CurrentUser::deallocate()
    {
        delete current_user_singleton;
        current_user_singleton = NULL;
    }   // deallocate
    // ------------------------------------------------------------------------
    /** Adds the login credential to a http request. A handy static function
     *  to allow for shorter request creation code. It sets the name of
     *  the script to invokce, token, and user id.
     *  \param request The http request.
     */
    void CurrentUser::setUserDetails(HTTPRequest *request)
    {
        CurrentUser *cu = CurrentUser::get();
        assert(cu && cu->m_state == US_SIGNED_IN);
        assert(cu->m_profile);
        request->setServerURL("client-user.php");
        request->addParameter("token", cu->m_token);
        request->addParameter("userid", cu->m_profile->getID());
    }   // setUserDetails

    // ========================================================================
    CurrentUser::CurrentUser()
    {
        m_state        = US_SIGNED_OUT;
        m_token        = "";
        m_save_session = false;
        m_profile      = NULL;
    }   // CurrentUser

    // ------------------------------------------------------------------------
    /** Request a login using the saved credentials of the user.
     */
    void CurrentUser::requestSavedSession()
    {
        SignInRequest * request = NULL;
        if(m_state == US_SIGNED_OUT  && UserConfigParams::m_saved_session)
        {
            request = new SignInRequest(true);
            request->setServerURL("client-user.php");
            request->addParameter("action","saved-session");
            request->addParameter("userid", UserConfigParams::m_saved_user);
            request->addParameter("token", 
                                  UserConfigParams::m_saved_token.c_str());
            request->queue();
            m_state = US_SIGNING_IN;
        }
    }   // requestSavedSession

    // ------------------------------------------------------------------------
    /** Create a signin request.
     *  \param username Name of user.
     *  \param password Password.
     *  \param save_session If true, the login credential will be saved to
     *         allow a password-less login.
     *  \param request_now Immediately submit this request to the
     *         RequestManager.
     */
    CurrentUser::SignInRequest*
        CurrentUser::requestSignIn(const core::stringw &username,
                                   const core::stringw &password,
                                   bool save_session, bool request_now)
    {
        assert(m_state == US_SIGNED_OUT);
        m_save_session = save_session;
        SignInRequest * request = new SignInRequest(false);
        request->setServerURL("client-user.php");
        request->addParameter("action","connect");
        request->addParameter("username",username);
        request->addParameter("password",password);
        request->addParameter("save-session", save_session);
        if (request_now)
        {
            request->queue();
            m_state = US_SIGNING_IN;
        }
        return request;
    }   // requestSignIn

    // ------------------------------------------------------------------------
    /** Called when the signin request is finished.
     */
    void CurrentUser::SignInRequest::callback()
    {
        CurrentUser::get()->signIn(isSuccess(), getXMLData());
        GUIEngine::Screen *screen = GUIEngine::getCurrentScreen();
        LoginScreen *login = dynamic_cast<LoginScreen*>(screen);
        if(login)
        {
            if(isSuccess())
                login->loginSuccessful();
            else
                login->loginError(getInfo());
        }   // if dialog
    }   // SignInRequest::callback

    // ------------------------------------------------------------------------
    /** Checks the server respond after a login attempt. If the login
     *  was successful, it marks the user as logged in, and (if requested)
     *  saves data to be able to login next time.
     *  \param success If the answer from the server indicated a 
     *         successful login attemp.
     *  \param input Xml tree with the complete server response.
     */
    void CurrentUser::signIn(bool success, const XMLNode * input)
    {
        if (success)
        {
            int token_fetched       = input->get("token", &m_token);
            core::stringw username("");
            int username_fetched    = input->get("username", &username);
            uint32_t userid(0);
            int userid_fetched      = input->get("userid", &userid);
            m_profile = new OnlineProfile(userid, username, true);
            assert(token_fetched && username_fetched && userid_fetched);
            m_state = US_SIGNED_IN;
            if(saveSession())
            {
                UserConfigParams::m_saved_user    = getID();
                UserConfigParams::m_saved_token   = getToken();
                UserConfigParams::m_saved_session = true;
            }
            ProfileManager::get()->addPersistent(m_profile);
            std::string achieved_string("");
            if(input->get("achieved", &achieved_string) == 1)
            {
                std::vector<uint32_t> achieved_ids = 
                    StringUtils::splitToUInt(achieved_string, ' ');
                PlayerManager::getCurrentAchievementsStatus()->sync(achieved_ids);
            }
            m_profile->fetchFriends();
        }   // if success
        else
        {
            m_state = US_SIGNED_OUT;
        }
    }   // signIn

    // ------------------------------------------------------------------------
    void CurrentUser::requestSignOut()
    {
        assert(m_state == US_SIGNED_IN || m_state == US_GUEST);
        SignOutRequest * request = new SignOutRequest();
        request->setServerURL("client-user.php");
        request->addParameter("action","disconnect");
        request->addParameter("token", getToken());
        request->addParameter("userid", getID());
        request->queue();
        m_state = US_SIGNING_OUT;
    }   // requestSignOut

    // --------------------------------------------------------------------
    void CurrentUser::SignOutRequest::callback()
    {
        CurrentUser::get()->signOut(isSuccess(), getXMLData());
    }   // SignOutRequest::callback

    // ------------------------------------------------------------------------
    void CurrentUser::signOut(bool success, const XMLNode * input)
    {
        if(!success)
        {
            Log::warn("CurrentUser::signOut", "%s",
                      "There were some connection issues while signing out. "
                      "Report a bug if this caused issues.");
        }
        m_token = "";
        ProfileManager::get()->clearPersistent();
        m_profile = NULL;
        m_state = US_SIGNED_OUT;
        UserConfigParams::m_saved_user = 0;
        UserConfigParams::m_saved_token = "";
        UserConfigParams::m_saved_session = false;
    }   // signOut

    // ------------------------------------------------------------------------
    CurrentUser::ServerJoinRequest* 
                             CurrentUser::requestServerJoin(uint32_t server_id,
                                                            bool request_now)
    {
        assert(m_state == US_SIGNED_IN || m_state == US_GUEST);
        ServerJoinRequest * request = new ServerJoinRequest();
        request->setServerURL("address-management.php");
        request->addParameter("action","request-connection");
        request->addParameter("token", getToken());
        request->addParameter("id", getID());
        request->addParameter("server_id", server_id);
        if (request_now)
            request->queue();
        return request;
    }   // requestServerJoin

    // ------------------------------------------------------------------------
    void CurrentUser::ServerJoinRequest::callback()
    {
        if(isSuccess())
        {
            uint32_t server_id;
            getXMLData()->get("serverid", &server_id);
            ServersManager::get()->setJoinedServer(server_id);
        }
        //FIXME needs changes for actual valid joining
    }   // ServerJoinRequest::callback

    // ------------------------------------------------------------------------
    const XMLRequest* 
           CurrentUser::requestGetAddonVote(const std::string & addon_id) const
    {
        assert(m_state == US_SIGNED_IN);
        XMLRequest * request = new XMLRequest();
        request->setServerURL("client-user.php");
        request->addParameter("action", "get-addon-vote");
        request->addParameter("token", getToken());
        request->addParameter("userid", getID());
        request->addParameter("addonid", addon_id.substr(6));
        request->queue();
        return request;
    }   // requestGetAddonVote

    // ------------------------------------------------------------------------
    /** A request to the server, to perform a vote on an addon.
     *  \param addon_id the id of the addon to vote for.
     *  \param rating the voted rating.
     */
    const CurrentUser::SetAddonVoteRequest*
                 CurrentUser::requestSetAddonVote(const std::string & addon_id,
                                                  float rating) const
    {
        assert(m_state == US_SIGNED_IN);
        CurrentUser::SetAddonVoteRequest * request =
                                        new CurrentUser::SetAddonVoteRequest();
        request->setServerURL("client-user.php");
        request->addParameter("action", "set-addon-vote");
        request->addParameter("token", getToken());
        request->addParameter("userid", getID());
        request->addParameter("addonid", addon_id.substr(6));
        request->addParameter("rating", rating);
        request->queue();
        return request;
    }   // requestSetAddonVote

    // ------------------------------------------------------------------------
    /** Callback for the request to vote for an addon. Updates the local
     *  average rating.
     */
    void CurrentUser::SetAddonVoteRequest::callback()
    {
        if(isSuccess())
        {
            std::string addon_id;
            getXMLData()->get("addon-id", &addon_id);
            float average;
            getXMLData()->get("new-average", &average);
            addons_manager->getAddon(Addon::createAddonId(addon_id))
                          ->setRating(average);
        }
    }   // SetAddonVoteRequest::callback

    // ------------------------------------------------------------------------
    /** A request to the server, to fetch matching results for the supplied
     *  search term.
     *  \param search_string the string to search for.
     */
    XMLRequest*
        CurrentUser::requestUserSearch(const core::stringw &search_string) const
    {
        assert(m_state == US_SIGNED_IN);
        XMLRequest * request = new XMLRequest();
        request->setServerURL("client-user.php");
        request->addParameter("action", "user-search");
        request->addParameter("token", getToken());
        request->addParameter("userid", getID());
        request->addParameter("search-string", search_string);
        request->queue();
        return request;
    }   // requestUserSearch

    // ------------------------------------------------------------------------
    void CurrentUser::requestCancelFriend(const uint32_t friend_id) const
    {
        assert(m_state == US_SIGNED_IN);
        CurrentUser::CancelFriendRequest * request =
                                        new CurrentUser::CancelFriendRequest();
        request->setServerURL("client-user.php");
        request->addParameter("action", "cancel-friend-request");
        request->addParameter("token", getToken());
        request->addParameter("userid", getID());
        request->addParameter("friendid", friend_id);
        request->queue();
    }   // requestCancelFriend

    // ------------------------------------------------------------------------
    /** Callback for the request to cancel a friend invitation. Shows a
     *  confirmation message and takes care of updating all the cached
     *  information.
     */
    void CurrentUser::CancelFriendRequest::callback()
    {
        uint32_t id(0);
        getXMLData()->get("friendid", &id);
        core::stringw info_text("");
        if(isSuccess())
        {
            CurrentUser::get()->getProfile()->removeFriend(id);
            ProfileManager::get()->moveToCache(id);
            ProfileManager::get()->getProfileByID(id)->deleteRelationalInfo();
            OnlineProfileFriends::getInstance()->refreshFriendsList();
            info_text = _("Friend request cancelled!");
        }
        else
            info_text = getInfo();
        UserInfoDialog *dia = new UserInfoDialog(id, info_text,!isSuccess(),
                                                 true);
        GUIEngine::DialogQueue::get()->pushDialog(dia, true);
    }   // CancelFriendRequest::callback

    // ------------------------------------------------------------------------
    /** A request to the server, to remove a friend relation.
     *  \param friend_id The id of the friend to be removed.
     */
    void CurrentUser::requestRemoveFriend(const uint32_t friend_id) const
    {
        assert(m_state == US_SIGNED_IN);
        CurrentUser::RemoveFriendRequest * request = 
                                  new CurrentUser::RemoveFriendRequest(friend_id);
        request->setServerURL("client-user.php");
        request->addParameter("action", "remove-friend");
        request->addParameter("token", getToken());
        request->addParameter("userid", getID());
        request->addParameter("friendid", friend_id);
        request->queue();
    }   // requestRemoveFriend

    // ------------------------------------------------------------------------
    /** Callback for the request to remove a friend. Shows a confirmation
     *  message and takes care of updating all the cached information.
     */
    void CurrentUser::RemoveFriendRequest::callback()
    {
        core::stringw info_text("");
        if(isSuccess())
        {
            CurrentUser::get()->getProfile()->removeFriend(m_id);
            ProfileManager::get()->moveToCache(m_id);
            ProfileManager::get()->getProfileByID(m_id)->deleteRelationalInfo();
            OnlineProfileFriends::getInstance()->refreshFriendsList();
            info_text = _("Friend removed!");
        }
        else
            info_text = getInfo();
        UserInfoDialog *info = new UserInfoDialog(m_id, info_text,!isSuccess(), 
                                                  true);
        GUIEngine::DialogQueue::get()->pushDialog(info, true);

    }   // RemoveFriendRequest::callback

    // ------------------------------------------------------------------------
    /** A request to the server, to change the password of the signed in user.
     *  \param current_password The active password of the currently signed in
     *         user.
     *  \param new_password     The password the user wants to change to.
     *  \param new_password_ver Confirmation of that password. Has to be the
     *         exact same.
     */
    void CurrentUser::requestPasswordChange(const core::stringw &current_password,
                                            const core::stringw &new_password,
                                     const core::stringw &new_password_ver) const
    {
        assert(m_state == US_SIGNED_IN);
        ChangePasswordRequest * request = new ChangePasswordRequest();
        request->setServerURL("client-user.php");
        request->addParameter("action", "change_password");
        request->addParameter("userid", getID());
        request->addParameter("current", current_password);
        request->addParameter("new1", new_password);
        request->addParameter("new2", new_password_ver);
        request->queue();
    }   // requestPasswordChange
    // ------------------------------------------------------------------------
    /** Callback for the change password request. If the matching dialog is 
     *  still open, show a confirmation message.
     */
    void CurrentUser::ChangePasswordRequest::callback()
    {
        if(GUIEngine::ModalDialog::isADialogActive())
        {
            ChangePasswordDialog * dialog  = 
                dynamic_cast<ChangePasswordDialog*>(GUIEngine::ModalDialog
                                                             ::getCurrent());
            if(dialog != NULL)
            {
                if(isSuccess())
                    dialog->success();
                else
                    dialog->error(getInfo());
            }
        }
    }   // ChangePasswordRequest::callback

    // ------------------------------------------------------------------------
    /** Sends a request to the server to see if any new information is
     *  available. (online friends, notifications, etc.).
     */
    void CurrentUser::requestPoll() const
    {
        assert(m_state == US_SIGNED_IN);
        CurrentUser::PollRequest * request = new CurrentUser::PollRequest();
        request->setServerURL("client-user.php");
        request->addParameter("action", "poll");
        request->addParameter("token", getToken());
        request->addParameter("userid", getID());
        request->queue();
    }   // requestPoll()

    // ------------------------------------------------------------------------
    /** Callback for the poll request. Parses the information and spawns
     *  notifications accordingly.
     */
    void CurrentUser::PollRequest::callback()
    {
        if(isSuccess())
        {
            if(!CurrentUser::get()->isRegisteredUser())
                return;
            if(CurrentUser::get()->getProfile()->hasFetchedFriends())
            {
                std::string online_friends_string("");
                if(getXMLData()->get("online", &online_friends_string) == 1)
                {
                    std::vector<uint32_t> online_friends = 
                          StringUtils::splitToUInt(online_friends_string, ' ');
                    bool went_offline = false;
                    std::vector<uint32_t> friends = 
                                CurrentUser::get()->getProfile()->getFriends();
                    std::vector<core::stringw> to_notify;
                    for(unsigned int i = 0; i < friends.size(); ++i)
                    {
                         bool now_online = false;
                         std::vector<uint32_t>::iterator iter =
                             std::find(online_friends.begin(),
                                       online_friends.end(), friends[i]);
                         if (iter != online_friends.end())
                         {
                             now_online = true;
                             online_friends.erase(iter);
                         }
                         OnlineProfile * profile =
                             ProfileManager::get()->getProfileByID(friends[i]);
                         OnlineProfile::RelationInfo * relation_info = 
                                                    profile->getRelationInfo();
                         if( relation_info->isOnline() )
                         {
                             if (!now_online)
                             {
                                 relation_info->setOnline(false);
                                 went_offline = true;
                             }
                         }
                         else
                         {
                             if (now_online)
                             {
                                 //User came online
                                 relation_info->setOnline(true);
                                 // Do this because a user might have accepted
                                 // a pending friend request.
                                 profile->setFriend(); 
                                 to_notify.push_back(profile->getUserName());
                             }
                         }

                    }

                    if(to_notify.size() > 0)
                    {
                        core::stringw message("");
                        if(to_notify.size() == 1)
                        {
                            message = _("%s is now online.", to_notify[0]);
                        }
                        else if(to_notify.size() == 2)
                        {
                            message = _("%s and %s are now online.",
                                        to_notify[0], to_notify[1]    );
                        }
                        else if(to_notify.size() == 3)
                        {
                            message = _("%s, %s and %s are now online.",
                                     to_notify[0], to_notify[1], to_notify[2]);
                        }
                        else if(to_notify.size() > 3)
                        {
                            message = _("%d friends are now online.", 
                                        to_notify.size());
                        }
                        NotificationDialog *dia = 
                            new NotificationDialog(NotificationDialog::T_Friends,
                                                   message);
                        GUIEngine::DialogQueue::get()->pushDialog(dia, false);
                        OnlineProfileFriends::getInstance()->refreshFriendsList();
                    }
                    else if(went_offline)
                    {
                        OnlineProfileFriends::getInstance()->refreshFriendsList();
                    }
                }
            }
            else
            {
                CurrentUser::get()->getProfile()->fetchFriends();
            }

            int friend_request_count = 0;
            for(unsigned int i = 0; i < getXMLData()->getNumNodes(); i++)
            {
                const XMLNode * node = getXMLData()->getNode(i);
                if(node->getName() == "new_friend_request")
                {
                    OnlineProfile::RelationInfo * ri = 
                        new OnlineProfile::RelationInfo("New", false, true, true);
                    OnlineProfile * p = new OnlineProfile(node);
                    p->setRelationInfo(ri);
                    ProfileManager::get()->addPersistent(p);
                    friend_request_count++;
                }
            }
            if(friend_request_count > 0)
            {
                core::stringw message("");
                if(friend_request_count > 1)
                {
                    message = _("You have %d new friend requests!",
                                friend_request_count);
                }
                else
                {
                    message = _("You have a new friend request!");
                }
                NotificationDialog *dia =
                    new NotificationDialog(NotificationDialog::T_Friends, 
                                           message);
                GUIEngine::DialogQueue::get()->pushDialog(dia, false);
                OnlineProfileFriends::getInstance()->refreshFriendsList();
            }
        }
        // FIXME show connection error??
        // Perhaps show something after 2 misses.

    }   // PollRequest::callback

    // ------------------------------------------------------------------------
    /** Sends a message to the server that the client has been closed, if a
     *  user is signed in.
     */
    void CurrentUser::onSTKQuit() const
    {
        if(isRegisteredUser())
        {
            HTTPRequest * request =
                      new HTTPRequest(true, RequestManager::HTTP_MAX_PRIORITY);
            request->setServerURL("client-user.php");
            request->addParameter("action", "client-quit");
            request->addParameter("token", getToken());
            request->addParameter("userid", getID());
            request->queue();
        }
    }

    // ------------------------------------------------------------------------
    /** Sends a confirmation to the server that an achievement has been
     *  completed, if a user is signed in.
     *  \param achievement_id the id of the achievement that got completed.
     */
    void CurrentUser::onAchieving(uint32_t achievement_id) const
    {
        if(isRegisteredUser())
        {
            HTTPRequest * request = new HTTPRequest(true);
            request->setServerURL("client-user.php");
            request->addParameter("action", "achieving");
            request->addParameter("token", getToken());
            request->addParameter("userid", getID());
            request->addParameter("achievementid", achievement_id);
            request->queue();
        }
    }   // onAchieving

    // ------------------------------------------------------------------------
    /** \return the username if signed in. */
    core::stringw CurrentUser::getUserName() const
    {
        if((m_state == US_SIGNED_IN ) || (m_state == US_GUEST))
        {
            assert(m_profile != NULL);
            return m_profile->getUserName();
        }
        return _("Currently not signed in");
    }   // getUserName

    // ------------------------------------------------------------------------
    /** \return the online id, or 0 if the user is not signed in.
     */ 
    uint32_t CurrentUser::getID() const
    {
        if((m_state == US_SIGNED_IN ))
        {
            assert(m_profile != NULL);
            return m_profile->getID();
        }
        return 0;
    }   // getID

} // namespace Online
