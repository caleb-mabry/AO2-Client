#include "aooptionsdialog.h"
#include "aoapplication.h"
#include "bass.h"

AOOptionsDialog::AOOptionsDialog(QWidget *parent, AOApplication *p_ao_app) : QDialog(parent)
{
    ao_app = p_ao_app;

    // Setting up the basics.
    // setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Settings");
    resize(398, 320);

    SettingsButtons = new QDialogButtonBox(this);

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(SettingsButtons->sizePolicy().hasHeightForWidth());
    SettingsButtons->setSizePolicy(sizePolicy1);
    SettingsButtons->setOrientation(Qt::Horizontal);
    SettingsButtons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);

    QObject::connect(SettingsButtons, SIGNAL(accepted()), this, SLOT(save_pressed()));
    QObject::connect(SettingsButtons, SIGNAL(rejected()), this, SLOT(discard_pressed()));

    // We'll stop updates so that the window won't flicker while it's being made.
    setUpdatesEnabled(false);

    // First of all, we want a tabbed dialog, so let's add some layout.
    verticalLayout = new QVBoxLayout(this);
    SettingsTabs = new QTabWidget(this);

    verticalLayout->addWidget(SettingsTabs);
    verticalLayout->addWidget(SettingsButtons);

    // Let's add the tabs one by one.
    // First, we'll start with 'Gameplay'.
    GameplayTab = new QWidget();
    SettingsTabs->addTab(GameplayTab, "Gameplay");

    formLayoutWidget = new QWidget(GameplayTab);
    formLayoutWidget->setGeometry(QRect(10, 10, 361, 211));

    GameplayForm = new QFormLayout(formLayoutWidget);
    GameplayForm->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    GameplayForm->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    GameplayForm->setContentsMargins(0, 0, 0, 0);

    ThemeLabel = new QLabel(formLayoutWidget);
    ThemeLabel->setText("Theme:");
    ThemeLabel->setToolTip("Allows you to set the theme used ingame. If your theme changes the lobby's look, too, you'll obviously need to reload the lobby somehow for it take effect. Joining a server and leaving it should work.");
    GameplayForm->setWidget(0, QFormLayout::LabelRole, ThemeLabel);

    ThemeCombobox = new QComboBox(formLayoutWidget);

    // Fill the combobox with the names of the themes.
    QDirIterator it(p_ao_app->get_base_path() + "themes", QDir::Dirs, QDirIterator::NoIteratorFlags);
    while (it.hasNext())
    {
        QString actualname = QDir(it.next()).dirName();
        if (actualname != "." && actualname != "..")
            ThemeCombobox->addItem(actualname);
        if (actualname == p_ao_app->read_theme())
            ThemeCombobox->setCurrentIndex(ThemeCombobox->count()-1);
    }

    GameplayForm->setWidget(0, QFormLayout::FieldRole, ThemeCombobox);

    ThemeLogDivider = new QFrame(formLayoutWidget);
    ThemeLogDivider->setMidLineWidth(0);
    ThemeLogDivider->setFrameShape(QFrame::HLine);
    ThemeLogDivider->setFrameShadow(QFrame::Sunken);

    GameplayForm->setWidget(1, QFormLayout::FieldRole, ThemeLogDivider);

    DownwardsLabel = new QLabel(formLayoutWidget);
    DownwardsLabel->setText("Log goes downwards:");
    DownwardsLabel->setToolTip("If ticked, the IC chatlog will go downwards, in the sense that new messages will appear at the bottom (like the OOC chatlog). The Vanilla behaviour is equivalent to this being unticked.");

    GameplayForm->setWidget(2, QFormLayout::LabelRole, DownwardsLabel);

    DownwardCheckbox = new QCheckBox(formLayoutWidget);
    DownwardCheckbox->setChecked(p_ao_app->get_log_goes_downwards());

    GameplayForm->setWidget(2, QFormLayout::FieldRole, DownwardCheckbox);

    LengthLabel = new QLabel(formLayoutWidget);
    LengthLabel->setText("Log length:");
    LengthLabel->setToolTip("The amount of messages the IC chatlog will keep before getting rid of older messages. A value of 0 or below counts as 'infinite'.");

    GameplayForm->setWidget(3, QFormLayout::LabelRole, LengthLabel);

    LengthSpinbox = new QSpinBox(formLayoutWidget);
    LengthSpinbox->setMaximum(10000);
    LengthSpinbox->setValue(p_ao_app->get_max_log_size());

    GameplayForm->setWidget(3, QFormLayout::FieldRole, LengthSpinbox);

    LogNamesDivider = new QFrame(formLayoutWidget);
    LogNamesDivider->setFrameShape(QFrame::HLine);
    LogNamesDivider->setFrameShadow(QFrame::Sunken);

    GameplayForm->setWidget(4, QFormLayout::FieldRole, LogNamesDivider);

    UsernameLabel = new QLabel(formLayoutWidget);
    UsernameLabel->setText("Default username:");
    UsernameLabel->setToolTip("Your OOC name will be filled in with this string when you join a server.");

    GameplayForm->setWidget(5, QFormLayout::LabelRole, UsernameLabel);

    UsernameLineEdit = new QLineEdit(formLayoutWidget);
    UsernameLineEdit->setMaxLength(30);
    UsernameLineEdit->setText(p_ao_app->get_default_username());

    GameplayForm->setWidget(5, QFormLayout::FieldRole, UsernameLineEdit);

    ShownameLabel = new QLabel(formLayoutWidget);
    ShownameLabel->setText("Custom shownames:");
    ShownameLabel->setToolTip("Gives the default value for the ingame 'Custom shownames' tickbox, which in turn determines whether your client should display custom shownames or not.");

    GameplayForm->setWidget(6, QFormLayout::LabelRole, ShownameLabel);

    ShownameCheckbox = new QCheckBox(formLayoutWidget);
    ShownameCheckbox->setChecked(p_ao_app->get_showname_enabled_by_default());

    GameplayForm->setWidget(6, QFormLayout::FieldRole, ShownameCheckbox);

    NetDivider = new QFrame(formLayoutWidget);
    NetDivider->setFrameShape(QFrame::HLine);
    NetDivider->setFrameShadow(QFrame::Sunken);

    GameplayForm->setWidget(7, QFormLayout::FieldRole, NetDivider);

    MasterServerLabel = new QLabel(formLayoutWidget);
    MasterServerLabel->setText("Backup MS:");
    MasterServerLabel->setToolTip("After the built-in server lookups fail, the game will try the address given here and use it as a backup masterserver address.");

    GameplayForm->setWidget(8, QFormLayout::LabelRole, MasterServerLabel);

    QSettings* configini = ao_app->configini;
    MasterServerLineEdit = new QLineEdit(formLayoutWidget);
    MasterServerLineEdit->setText(configini->value("master", "").value<QString>());

    GameplayForm->setWidget(8, QFormLayout::FieldRole, MasterServerLineEdit);

    DiscordLabel = new QLabel(formLayoutWidget);
    DiscordLabel->setText("Discord:");
    DiscordLabel->setToolTip("If true, allows Discord's Rich Presence to read data about your game. These are: what server you are in, what character are you playing, and how long have you been playing for.");

    GameplayForm->setWidget(9, QFormLayout::LabelRole, DiscordLabel);

    DiscordCheckBox = new QCheckBox(formLayoutWidget);
    DiscordCheckBox->setChecked(ao_app->is_discord_enabled());

    GameplayForm->setWidget(9, QFormLayout::FieldRole, DiscordCheckBox);

    // Here we start the callwords tab.
    CallwordsTab = new QWidget();
    SettingsTabs->addTab(CallwordsTab, "Callwords");

    verticalLayoutWidget = new QWidget(CallwordsTab);
    verticalLayoutWidget->setGeometry(QRect(10, 10, 361, 211));

    CallwordsLayout = new QVBoxLayout(verticalLayoutWidget);
    CallwordsLayout->setContentsMargins(0,0,0,0);

    CallwordsTextEdit = new QPlainTextEdit(verticalLayoutWidget);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(CallwordsTextEdit->sizePolicy().hasHeightForWidth());
    CallwordsTextEdit->setSizePolicy(sizePolicy);

    // Let's fill the callwords text edit with the already present callwords.
    CallwordsTextEdit->document()->clear();
    foreach (QString callword, p_ao_app->get_call_words()) {
        CallwordsTextEdit->appendPlainText(callword);
    }

    CallwordsLayout->addWidget(CallwordsTextEdit);

    CallwordsExplainLabel = new QLabel(verticalLayoutWidget);
    CallwordsExplainLabel->setWordWrap(true);
    CallwordsExplainLabel->setText("<html><head/><body>Enter as many callwords as you would like. These are case insensitive. Make sure to leave every callword in its own line!<br>Do not leave a line with a space at the end -- you will be alerted everytime someone uses a space in their messages.</body></html>");

    CallwordsLayout->addWidget(CallwordsExplainLabel);

    // The audio tab.
    AudioTab = new QWidget();
    SettingsTabs->addTab(AudioTab, "Audio");

    formLayoutWidget_2 = new QWidget(AudioTab);
    formLayoutWidget_2->setGeometry(QRect(10, 10, 361, 211));

    AudioForm = new QFormLayout(formLayoutWidget_2);
    AudioForm->setObjectName(QStringLiteral("AudioForm"));
    AudioForm->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    AudioForm->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    AudioForm->setContentsMargins(0, 0, 0, 0);

    AudioDevideLabel = new QLabel(formLayoutWidget_2);
    AudioDevideLabel->setText("Audio device:");
    AudioDevideLabel->setToolTip("Allows you to set the theme used ingame. If your theme changes the lobby's look, too, you'll obviously need to reload the lobby somehow for it take effect. Joining a server and leaving it should work.");

    AudioForm->setWidget(0, QFormLayout::LabelRole, AudioDevideLabel);

    AudioDeviceCombobox = new QComboBox(formLayoutWidget_2);

    // Let's fill out the combobox with the available audio devices.
    int a = 0;
    BASS_DEVICEINFO info;

    if (needs_default_audiodev())
    {
        AudioDeviceCombobox->addItem("Default");
    }

    for (a = 0; BASS_GetDeviceInfo(a, &info); a++)
    {
        AudioDeviceCombobox->addItem(info.name);
        if (p_ao_app->get_audio_output_device() == info.name)
            AudioDeviceCombobox->setCurrentIndex(AudioDeviceCombobox->count()-1);
    }

    AudioForm->setWidget(0, QFormLayout::FieldRole, AudioDeviceCombobox);

    DeviceVolumeDivider = new QFrame(formLayoutWidget_2);
    DeviceVolumeDivider->setFrameShape(QFrame::HLine);
    DeviceVolumeDivider->setFrameShadow(QFrame::Sunken);

    AudioForm->setWidget(1, QFormLayout::FieldRole, DeviceVolumeDivider);

    MusicVolumeLabel = new QLabel(formLayoutWidget_2);
    MusicVolumeLabel->setText("Music:");
    MusicVolumeLabel->setToolTip("Sets the music's default volume.");

    AudioForm->setWidget(2, QFormLayout::LabelRole, MusicVolumeLabel);

    MusicVolumeSpinbox = new QSpinBox(formLayoutWidget_2);
    MusicVolumeSpinbox->setValue(p_ao_app->get_default_music());
    MusicVolumeSpinbox->setMaximum(100);
    MusicVolumeSpinbox->setSuffix("%");

    AudioForm->setWidget(2, QFormLayout::FieldRole, MusicVolumeSpinbox);

    SFXVolumeLabel = new QLabel(formLayoutWidget_2);
    SFXVolumeLabel->setText("SFX:");
    SFXVolumeLabel->setToolTip("Sets the SFX's default volume. Interjections and actual sound effects count as 'SFX'.");

    AudioForm->setWidget(3, QFormLayout::LabelRole, SFXVolumeLabel);

    SFXVolumeSpinbox = new QSpinBox(formLayoutWidget_2);
    SFXVolumeSpinbox->setValue(p_ao_app->get_default_sfx());
    SFXVolumeSpinbox->setMaximum(100);
    SFXVolumeSpinbox->setSuffix("%");

    AudioForm->setWidget(3, QFormLayout::FieldRole, SFXVolumeSpinbox);

    BlipsVolumeLabel = new QLabel(formLayoutWidget_2);
    BlipsVolumeLabel->setText("Blips:");
    BlipsVolumeLabel->setToolTip("Sets the volume of the blips, the talking sound effects.");

    AudioForm->setWidget(4, QFormLayout::LabelRole, BlipsVolumeLabel);

    BlipsVolumeSpinbox = new QSpinBox(formLayoutWidget_2);
    BlipsVolumeSpinbox->setValue(p_ao_app->get_default_blip());
    BlipsVolumeSpinbox->setMaximum(100);
    BlipsVolumeSpinbox->setSuffix("%");

    AudioForm->setWidget(4, QFormLayout::FieldRole, BlipsVolumeSpinbox);

    VolumeBlipDivider = new QFrame(formLayoutWidget_2);
    VolumeBlipDivider->setFrameShape(QFrame::HLine);
    VolumeBlipDivider->setFrameShadow(QFrame::Sunken);

    AudioForm->setWidget(5, QFormLayout::FieldRole, VolumeBlipDivider);

    BlipRateLabel = new QLabel(formLayoutWidget_2);
    BlipRateLabel->setText("Blip rate:");
    BlipRateLabel->setToolTip("Sets the delay between playing the blip sounds.");

    AudioForm->setWidget(6, QFormLayout::LabelRole, BlipRateLabel);

    BlipRateSpinbox = new QSpinBox(formLayoutWidget_2);
    BlipRateSpinbox->setValue(p_ao_app->read_blip_rate());
    BlipRateSpinbox->setMinimum(1);

    AudioForm->setWidget(6, QFormLayout::FieldRole, BlipRateSpinbox);

    BlankBlipsLabel = new QLabel(formLayoutWidget_2);
    BlankBlipsLabel->setText("Blank blips:");
    BlankBlipsLabel->setToolTip("If true, the game will play a blip sound even when a space is 'being said'.");

    AudioForm->setWidget(7, QFormLayout::LabelRole, BlankBlipsLabel);

    BlankBlipsCheckbox = new QCheckBox(formLayoutWidget_2);
    BlankBlipsCheckbox->setChecked(p_ao_app->get_blank_blip());

    AudioForm->setWidget(7, QFormLayout::FieldRole, BlankBlipsCheckbox);

    // The casing tab!
    CasingTab = new QWidget();
    SettingsTabs->addTab(CasingTab, "Casing");

    formLayoutWidget_3 = new QWidget(CasingTab);
    formLayoutWidget_3->setGeometry(QRect(10,10, 361, 211));

    CasingForm = new QFormLayout(formLayoutWidget_3);
    CasingForm->setObjectName(QStringLiteral("CasingForm"));
    CasingForm->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    CasingForm->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    CasingForm->setContentsMargins(0, 0, 0, 0);

    // -- SERVER SUPPORTS CASING

    ServerSupportsCasing = new QLabel(formLayoutWidget_3);
    if (ao_app->casing_alerts_enabled)
      ServerSupportsCasing->setText("This server supports case alerts.");
    else
      ServerSupportsCasing->setText("This server does not support case alerts.");
    ServerSupportsCasing->setToolTip("Pretty self-explanatory.");

    CasingForm->setWidget(0, QFormLayout::FieldRole, ServerSupportsCasing);

    // -- CASE ANNOUNCEMENTS

    CasingEnabledLabel = new QLabel(formLayoutWidget_3);
    CasingEnabledLabel->setText("Casing:");
    CasingEnabledLabel->setToolTip("If checked, you will get alerts about case announcements.");

    CasingForm->setWidget(1, QFormLayout::LabelRole, CasingEnabledLabel);

    CasingEnabledCheckbox = new QCheckBox(formLayoutWidget_3);
    CasingEnabledCheckbox->setChecked(ao_app->get_casing_enabled());

    CasingForm->setWidget(1, QFormLayout::FieldRole, CasingEnabledCheckbox);

    // -- DEFENCE ANNOUNCEMENTS

    DefenceLabel = new QLabel(formLayoutWidget_3);
    DefenceLabel->setText("Defence:");
    DefenceLabel->setToolTip("If checked, you will get alerts about case announcements if a defence spot is open.");

    CasingForm->setWidget(2, QFormLayout::LabelRole, DefenceLabel);

    DefenceCheckbox = new QCheckBox(formLayoutWidget_3);
    DefenceCheckbox->setChecked(ao_app->get_casing_defence_enabled());

    CasingForm->setWidget(2, QFormLayout::FieldRole, DefenceCheckbox);

    // -- PROSECUTOR ANNOUNCEMENTS

    ProsecutorLabel = new QLabel(formLayoutWidget_3);
    ProsecutorLabel->setText("Prosecution:");
    ProsecutorLabel->setToolTip("If checked, you will get alerts about case announcements if a prosecutor spot is open.");

    CasingForm->setWidget(3, QFormLayout::LabelRole, ProsecutorLabel);

    ProsecutorCheckbox = new QCheckBox(formLayoutWidget_3);
    ProsecutorCheckbox->setChecked(ao_app->get_casing_prosecution_enabled());

    CasingForm->setWidget(3, QFormLayout::FieldRole, ProsecutorCheckbox);

    // -- JUDGE ANNOUNCEMENTS

    JudgeLabel = new QLabel(formLayoutWidget_3);
    JudgeLabel->setText("Judge:");
    JudgeLabel->setToolTip("If checked, you will get alerts about case announcements if the judge spot is open.");

    CasingForm->setWidget(4, QFormLayout::LabelRole, JudgeLabel);

    JudgeCheckbox = new QCheckBox(formLayoutWidget_3);
    JudgeCheckbox->setChecked(ao_app->get_casing_judge_enabled());

    CasingForm->setWidget(4, QFormLayout::FieldRole, JudgeCheckbox);

    // -- JUROR ANNOUNCEMENTS

    JurorLabel = new QLabel(formLayoutWidget_3);
    JurorLabel->setText("Juror:");
    JurorLabel->setToolTip("If checked, you will get alerts about case announcements if a juror spot is open.");

    CasingForm->setWidget(5, QFormLayout::LabelRole, JurorLabel);

    JurorCheckbox = new QCheckBox(formLayoutWidget_3);
    JurorCheckbox->setChecked(ao_app->get_casing_juror_enabled());

    CasingForm->setWidget(5, QFormLayout::FieldRole, JurorCheckbox);

    // -- STENO ANNOUNCEMENTS

    StenographerLabel = new QLabel(formLayoutWidget_3);
    StenographerLabel->setText("Stenographer:");
    StenographerLabel->setToolTip("If checked, you will get alerts about case announcements if a stenographer spot is open.");

    CasingForm->setWidget(6, QFormLayout::LabelRole, StenographerLabel);

    StenographerCheckbox = new QCheckBox(formLayoutWidget_3);
    StenographerCheckbox->setChecked(ao_app->get_casing_steno_enabled());

    CasingForm->setWidget(6, QFormLayout::FieldRole, StenographerCheckbox);

    // -- CM ANNOUNCEMENTS

    CMLabel = new QLabel(formLayoutWidget_3);
    CMLabel->setText("CM:");
    CMLabel->setToolTip("If checked, you will appear amongst the potential CMs on the server.");

    CasingForm->setWidget(7, QFormLayout::LabelRole, CMLabel);

    CMCheckbox = new QCheckBox(formLayoutWidget_3);
    CMCheckbox->setChecked(ao_app->get_casing_cm_enabled());

    CasingForm->setWidget(7, QFormLayout::FieldRole, CMCheckbox);

    // -- CM CASES ANNOUNCEMENTS

    CMCasesLabel = new QLabel(formLayoutWidget_3);
    CMCasesLabel->setText("Hosting cases:");
    CMCasesLabel->setToolTip("If you're a CM, enter what cases are you willing to host.");

    CasingForm->setWidget(8, QFormLayout::LabelRole, CMCasesLabel);

    CMCasesLineEdit = new QLineEdit(formLayoutWidget_3);
    CMCasesLineEdit->setText(ao_app->get_casing_can_host_cases());

    CasingForm->setWidget(8, QFormLayout::FieldRole, CMCasesLineEdit);

    // When we're done, we should continue the updates!
    setUpdatesEnabled(true);
}

void AOOptionsDialog::save_pressed()
{
    // Save everything into the config.ini.
    QSettings* configini = ao_app->configini;

    configini->setValue("theme", ThemeCombobox->currentText());
    configini->setValue("log_goes_downwards", DownwardCheckbox->isChecked());
    configini->setValue("log_maximum", LengthSpinbox->value());
    configini->setValue("default_username", UsernameLineEdit->text());
    configini->setValue("show_custom_shownames", ShownameCheckbox->isChecked());
    configini->setValue("master", MasterServerLineEdit->text());
    configini->setValue("discord", DiscordCheckBox->isChecked());

    QFile* callwordsini = new QFile(ao_app->get_base_path() + "callwords.ini");

    if (!callwordsini->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        // Nevermind!
    }
    else
    {
        QTextStream out(callwordsini);
        out << CallwordsTextEdit->toPlainText();
        callwordsini->close();
    }

    configini->setValue("default_audio_device", AudioDeviceCombobox->currentText());
    configini->setValue("default_music", MusicVolumeSpinbox->value());
    configini->setValue("default_sfx", SFXVolumeSpinbox->value());
    configini->setValue("default_blip", BlipsVolumeSpinbox->value());
    configini->setValue("blip_rate", BlipRateSpinbox->value());
    configini->setValue("blank_blip", BlankBlipsCheckbox->isChecked());

    configini->setValue("casing_enabled", CasingEnabledCheckbox->isChecked());
    configini->setValue("casing_defence_enabled", DefenceCheckbox->isChecked());
    configini->setValue("casing_prosecution_enabled", ProsecutorCheckbox->isChecked());
    configini->setValue("casing_judge_enabled", JudgeCheckbox->isChecked());
    configini->setValue("casing_juror_enabled", JurorCheckbox->isChecked());
    configini->setValue("casing_steno_enabled", StenographerCheckbox->isChecked());
    configini->setValue("casing_cm_enabled", CMCheckbox->isChecked());
    configini->setValue("casing_can_host_casees", CMCasesLineEdit->text());

    callwordsini->close();
    done(0);
}

void AOOptionsDialog::discard_pressed()
{
    done(0);
}

#if (defined (_WIN32) || defined (_WIN64))
bool AOOptionsDialog::needs_default_audiodev()
{
    return true;
}
#elif (defined (LINUX) || defined (__linux__))
bool AOOptionsDialog::needs_default_audiodev()
{
    return false;
}
#else
#error This operating system is not supported.
#endif
