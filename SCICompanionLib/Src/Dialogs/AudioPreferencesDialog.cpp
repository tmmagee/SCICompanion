#include "stdafx.h"
#include "AppState.h"
#include "AudioPreferencesDialog.h"
#include "AudioCacheResourceSource.h"
#include "MidiPlayer.h"

// AudioPreferencesDialog dialog

AudioPreferencesDialog::AudioPreferencesDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(AudioPreferencesDialog::IDD, pParent)
{
}

AudioPreferencesDialog::~AudioPreferencesDialog()
{
}

BOOL AudioPreferencesDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    // Add the MIDI devices
    UINT cDevices = midiOutGetNumDevs();
    for (UINT i = 0; i < cDevices; i++)
    {
        MIDIOUTCAPS outcaps = { 0 };
        MMRESULT result = midiOutGetDevCaps(i, &outcaps, sizeof(outcaps));
        if (result == MMSYSERR_NOERROR)
        {
            m_wndMIDIDevices.InsertString(-1, outcaps.szPname);
        }
    }
    if (cDevices)
    {
        m_wndMIDIDevices.SetCurSel(appState->GetMidiDeviceId());
    }
    return fRet;
}

void AudioPreferencesDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    DDX_Text(pDX, IDC_EDIT_TRIMLEFT, appState->_audioTrimLeft);
    DDV_MinMaxInt(pDX, appState->_audioTrimLeft, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_TRIMRIGHT, appState->_audioTrimRight);
    DDV_MinMaxInt(pDX, appState->_audioTrimRight, 0, 1000);

    DDX_Control(pDX, IDC_COMBO_MIDIDEVICE, m_wndMIDIDevices);

    // Visuals
    DDX_Control(pDX, IDC_GROUP2, m_wndGroup2);
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
    DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
    DDX_Control(pDX, IDC_BUTTON_REPACKAGE, m_wndButton1);
    DDX_Control(pDX, IDC_BUTTON_CLEARAUDIOCACHE, m_wndButton2);
}

BEGIN_MESSAGE_MAP(AudioPreferencesDialog, CExtResizableDialog)
    ON_CBN_SELCHANGE(IDC_COMBO_MIDIDEVICE, &AudioPreferencesDialog::OnCbnSelchangeComboMididevice)
    ON_BN_CLICKED(IDC_BUTTON_REPACKAGE, &AudioPreferencesDialog::OnBnClickedButtonRepackage)
    ON_BN_CLICKED(IDC_BUTTON_CLEARAUDIOCACHE, &AudioPreferencesDialog::OnBnClickedButtonClearAudioCache)
END_MESSAGE_MAP()


// AudioPreferencesDialog message handlers

void AudioPreferencesDialog::OnOK()
{
    __super::OnOK();
}

void AudioPreferencesDialog::OnCbnSelchangeComboMididevice()
{
    int selection = m_wndMIDIDevices.GetCurSel();
    if (selection != CB_ERR)
    {
        CString string;
        m_wndMIDIDevices.GetLBText(selection, string);
        appState->_midiDeviceName = (PCSTR)string;
        g_midiPlayer.Reset();
    }
}


void AudioPreferencesDialog::OnBnClickedButtonRepackage()
{
    appState->GetResourceMap().RepackageAudio(true);
}


void AudioPreferencesDialog::OnBnClickedButtonClearAudioCache()
{
    if (IDYES == AfxMessageBox("This will remove any audio working files. If audio files have not been repackaged into the game, you may lose data. Continue?", MB_YESNO | MB_ICONWARNING))
    {
        std::unique_ptr<AudioCacheResourceSource> cache = std::make_unique<AudioCacheResourceSource>(appState->GetVersion(), appState->GetResourceMap().GetGameFolder(), -1, ResourceSourceAccessFlags::ReadWrite);
        cache->Clear();
    }
}