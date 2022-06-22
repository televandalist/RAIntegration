#ifndef RA_DATA_MOCK_GAMECONTEXT_HH
#define RA_DATA_MOCK_GAMECONTEXT_HH
#pragma once

#include "data\context\GameContext.hh"

#include "services\ServiceLocator.hh"

namespace ra {
namespace data {
namespace context {
namespace mocks {

class MockGameContext : public GameContext
{
public:
    MockGameContext() noexcept
        : m_Override(this)
    {
    }

    void LoadGame(unsigned int nGameId, ra::data::context::GameContext::Mode nMode) noexcept override
    {
        m_nGameId = nGameId;
        m_nMode = nMode;
        m_bWasLoaded = true;
    }

    bool WasLoaded() const noexcept { return m_bWasLoaded; }
    void ResetWasLoaded() noexcept { m_bWasLoaded = false; }

    /// <summary>
    /// Sets the unique identifier of the currently loaded game.
    /// </summary>
    void SetGameId(unsigned int nGameId) noexcept { m_nGameId = nGameId; }

    void NotifyActiveGameChanged() { OnActiveGameChanged(); }

    void NotifyGameLoad() { BeginLoad(); EndLoad(); }

    /// <summary>
    /// Sets the rich presence display string.
    /// </summary>
    void SetRichPresenceDisplayString(std::wstring sValue)
    {
        auto* richPresence = Assets().FindRichPresence();
        if (richPresence == nullptr)
        {
            auto pRichPresence = std::make_unique<ra::data::models::RichPresenceModel>();
            pRichPresence->SetScript(ra::StringPrintf("Display:\n%s\n", sValue));
            pRichPresence->CreateServerCheckpoint();
            pRichPresence->CreateLocalCheckpoint();
            Assets().Append(std::move(pRichPresence));
        }
        else
        {
            richPresence->SetScript(ra::StringPrintf("Display:\n%s\n", sValue));
        }

        m_sRichPresenceDisplayString = sValue;
    }

    void SetRichPresenceFromFile(bool bValue)
    {
        auto* pRichPresence = Assets().FindRichPresence();
        if (pRichPresence)
        {
            if (bValue)
                pRichPresence->SetScript("Display:\nThis differs\n");
            else
                pRichPresence->SetScript(ra::StringPrintf("Display:\n%s\n", m_sRichPresenceDisplayString));
        }
    }

    bool SetCodeNote(ra::ByteAddress nAddress, const std::wstring& sNote) override
    {
        // non-API part of SetCodeNote
        AddCodeNote(nAddress, "Author", sNote);
        return true;
    }

    bool DeleteCodeNote(ra::ByteAddress nAddress) override
    {
        // non-API part of DeleteCodeNote
        m_mCodeNotes.erase(nAddress);
        OnCodeNoteChanged(nAddress, L"");
        return true;
    }

private:
    ra::services::ServiceLocator::ServiceOverride<ra::data::context::GameContext> m_Override;

    std::wstring m_sRichPresenceDisplayString;
    bool m_bHasActiveAchievements{ false };
    bool m_bWasLoaded{ false };
};

} // namespace mocks
} // namespace context
} // namespace data
} // namespace ra

#endif // !RA_DATA_MOCK_GAMECONTEXT_HH
