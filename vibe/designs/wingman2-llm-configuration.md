# Wingman2 LLM Configuration Feature Design

## Overview

This document describes the design for a new **Wingman2** LLM configuration feature in MindForger. The feature adds a new tab to the Workspace/Preferences dialog that provides an improved, structured approach to configuring Large Language Model (LLM) providers for use with Wingman.

### Goals

- Provide a clean, user-friendly interface for managing multiple LLM provider configurations
- Support adding, configuring, and selecting LLM providers (OpenAI, ollama)
- Enable probing/validation of LLM provider configurations
- Maintain backward compatibility with existing Wingman configuration
- Store multiple configured LLM providers and allow easy switching between them

### Non-Goals

- Modification or removal of existing Wingman tab (it stays as-is)
- Support for additional LLM providers beyond OpenAI and ollama
- Automatic provider discovery

## Architecture

### Component Overview

```
ConfigurationDialog
├── Existing Tabs (App, Viewer, Editor, etc.)
├── Wingman Tab (existing - unchanged)
└── Wingman2 Tab (NEW)
    ├── LLM Provider Dropdown (shows configured providers)
    ├── Add LLM Provider Button
    └── Help Text

Add LLM Provider Dialog (NEW)
├── Provider Selection (OpenAI, ollama)
└── Next Button → Opens Provider-Specific Config Dialog

OpenAI Configuration Dialog (NEW)
├── API Key Field
├── LLM Model Dropdown
├── Probe Button
├── Add Button
└── Cancel Button

ollama Configuration Dialog (NEW)
├── URL Field
├── LLM Model Field/Dropdown
├── Probe Button
├── Add Button
└── Cancel Button
```

## Data Model

### Constants

Default values for LLM provider configurations:

```cpp
// Default URLs
constexpr const auto DEFAULT_OLLAMA_URL = "http://localhost:11434";
constexpr const auto DEFAULT_OPENAI_API_URL = "https://api.openai.com/v1";

// Default models (already defined in configuration.h)
constexpr const auto LLM_MODEL_NONE = "";
constexpr const auto LLM_MODEL_GPT35_TURBO = "gpt-3.5-turbo";
constexpr const auto LLM_MODEL_GPT4 = "gpt-4";
constexpr const auto LLM_MODEL_PHI = "phi";
```

### Configuration Storage

New fields added to `Configuration` class (`lib/src/config/configuration.h`):

```cpp
// LLM provider configuration structure
struct LlmProviderConfig {
    std::string id;                    // unique identifier (e.g., "openai-1", "ollama-local")
    std::string displayName;           // user-friendly name (e.g., "OpenAI GPT-4", "Local Ollama")
    WingmanLlmProviders providerType;  // WINGMAN_PROVIDER_OPENAI, WINGMAN_PROVIDER_OLLAMA
    std::string url;                   // for ollama: base URL, for OpenAI: empty
    std::string apiKey;                // for OpenAI: API key, for ollama: empty
    std::string llmModel;              // model name (e.g., "gpt-4", "llama2")
    bool isValid;                      // whether configuration was validated/probed
    
    LlmProviderConfig() 
        : providerType(WINGMAN_PROVIDER_NONE), 
          isValid(false) {}
};

// In Configuration class:
private:
    // Collection of configured LLM providers
    std::vector<LlmProviderConfig> llmProviders;
    // Currently selected/active provider (ID from llmProviders)
    std::string activeLlmProviderId;
```

### Configuration File Persistence

The LLM provider configurations will be persisted in the `.mindforger.md` configuration file using a new Markdown section:

```markdown
## Wingman2 LLM Providers

Active Provider: openai-primary

### Provider: openai-primary
- Type: OpenAI
- Display Name: OpenAI GPT-4
- Model: gpt-4
- API Key: [encrypted or reference to env var]
- Valid: true

### Provider: ollama-local
- Type: ollama
- Display Name: Local Ollama
- Model: llama2
- URL: http://localhost:11434
- Valid: true
```

### Configuration API

New methods in `Configuration` class:

```cpp
// Provider management
std::vector<LlmProviderConfig>& getLlmProviders();
LlmProviderConfig* getLlmProviderById(const std::string& id);
LlmProviderConfig* getActiveLlmProvider();
void addLlmProvider(const LlmProviderConfig& provider);
void updateLlmProvider(const std::string& id, const LlmProviderConfig& provider);
void removeLlmProvider(const std::string& id);
void setActiveLlmProvider(const std::string& id);

// Provider validation
bool probeLlmProvider(const LlmProviderConfig& provider, std::string& errorMessage);
bool probeOpenAiProvider(const std::string& apiKey, const std::string& model, std::string& errorMessage);
bool probeOllamaProvider(const std::string& url, const std::string& model, std::string& errorMessage);

// Backward compatibility with existing Wingman configuration
void migrateFromLegacyWingmanConfig();
```

## UI Components

### 1. Wingman2 Tab (Main Configuration Panel)

**Location**: `app/src/qt/dialogs/configuration_dialog.h` and `.cpp`

**Class**: `ConfigurationDialog::Wingman2Tab`

**Layout**:
```
┌─ Wingman2 Tab ────────────────────────────────┐
│                                               │
│  Configure Large Language Model (LLM) to be  │
│  used by Wingman                              │
│                                               │
│  LLM Provider: [Dropdown ▼] [Add Provider]   │
│                                               │
│  ┌─ Selected Provider Details ─────────────┐ │
│  │                                          │ │
│  │  Provider Type: OpenAI                   │ │
│  │  Model: gpt-4                            │ │
│  │  Status: Configured ✓                    │ │
│  │                                          │ │
│  │  [Edit] [Remove] [Test Connection]      │ │
│  └──────────────────────────────────────────┘ │
│                                               │
└───────────────────────────────────────────────┘
```

**Fields**:
- `QLabel* helpLabel` - informational text
- `QComboBox* llmProvidersCombo` - dropdown showing configured providers
- `QPushButton* addProviderButton` - opens Add LLM Provider dialog
- `QGroupBox* providerDetailsGroup` - shows details of selected provider
- `QLabel* providerTypeLabel, *modelLabel, *statusLabel`
- `QPushButton* editButton, *removeButton, *testButton`

**Behavior**:
- On load: populate dropdown with configured providers from `config.getLlmProviders()`
- On provider selection: display details in provider details group
- On "Add Provider" click: open AddLlmProviderDialog
- On "Edit" click: open appropriate provider config dialog pre-filled
- On "Remove" click: confirm and remove provider from configuration
- On "Test Connection" click: run probe for selected provider

### 2. Add LLM Provider Dialog

**Location**: `app/src/qt/dialogs/add_llm_provider_dialog.h` and `.cpp`

**Class**: `AddLlmProviderDialog`

**Layout**:
```
┌─ New LLM Provider ────────────────────────────┐
│                                               │
│  Which provider do you want to configure?    │
│                                               │
│  Provider Type: [OpenAI        ▼]            │
│                                               │
│                           [Cancel] [Next >]  │
└───────────────────────────────────────────────┘
```

**Fields**:
- `QLabel* questionLabel`
- `QComboBox* providerTypeCombo` - options: "OpenAI", "ollama"
- `QPushButton* nextButton, *cancelButton`

**Behavior**:
- On "Next" click: 
  - If OpenAI selected → open OpenAiConfigDialog
  - If ollama selected → open OllamaConfigDialog
- On "Cancel" click: close dialog

### 3. OpenAI Configuration Dialog

**Location**: `app/src/qt/dialogs/openai_config_dialog.h` and `.cpp`

**Class**: `OpenAiConfigDialog`

**Layout**:
```
┌─ Configure OpenAI Provider ───────────────────┐
│                                               │
│  API Key: [________________]  [Reset]        │
│                                               │
│  Environment variable: MINDFORGER_OPENAI_API_KEY
│  (if set, overrides the value above)         │
│                                               │
│  LLM Model: [gpt-3.5-turbo ▼]  [Refresh]     │
│  (You can type model name or select from list)
│                                               │
│  [Probe] [Add] [Cancel]                      │
└───────────────────────────────────────────────┘
```

**Fields**:
- `QLineEdit* apiKeyEdit` - API key input (masked)
- `QPushButton* resetApiKeyButton` - reset to defaults
- `QLabel* envVarInfoLabel` - shows env var name
- `QComboBox* llmModelCombo` - model selection (editable)
- `QPushButton* refreshModelsButton` - fetch available models from OpenAI
- `QPushButton* probeButton, *addButton, *cancelButton`

**Behavior**:
- On "Reset" click:
  - Clear API key field
  - Set model combo to default ("gpt-3.5-turbo")
- On "Refresh" click:
  - Validate API key is set (from field or env var)
  - Create temporary `OpenAiWingman` instance
  - Call `listModels()` to fetch available models from OpenAI API
  - Populate `llmModelCombo` with results
  - Handle errors gracefully (show error message)
  - Note: OpenAI currently supports listing models via API
- On "Probe" click: 
  - Validate API key is set (from field or env var)
  - Validate model is set (typed or selected)
  - Call `config.probeOpenAiProvider(apiKey, model, errorMsg)`
  - Show success/error message
- On "Add" click:
  - Validate inputs (API key, model)
  - Create `LlmProviderConfig` with user inputs
  - Generate unique ID (e.g., "openai-{timestamp}")
  - Set display name (e.g., "OpenAI {model}")
  - Call `config.addLlmProvider(providerConfig)`
  - Close dialog
- On "Cancel" click: close dialog
- Model combo is **editable**: user can type custom model name or select from dropdown
  - Implementation: `llmModelCombo->setEditable(true);`
  - Get value: `llmModelCombo->currentText().toStdString()`

### 4. ollama Configuration Dialog

**Location**: `app/src/qt/dialogs/ollama_config_dialog.h` and `.cpp`

**Class**: `OllamaConfigDialog`

**Layout**:
```
┌─ Configure ollama Provider ───────────────────┐
│                                               │
│  ollama Server URL:                           │
│  [http://localhost:11434]  [Reset]           │
│                                               │
│  LLM Model: [llama2        ▼]  [Refresh]     │
│  (You can type model name or select from list)
│                                               │
│  [Probe] [Add] [Cancel]                      │
└───────────────────────────────────────────────┘
```

**Fields**:
- `QLineEdit* urlEdit` - ollama server URL
- `QPushButton* resetUrlButton` - reset to default URL
- `QComboBox* llmModelCombo` - model selection (editable)
- `QPushButton* refreshModelsButton` - fetch available models from server
- `QPushButton* probeButton, *addButton, *cancelButton`

**Behavior**:
- On "Reset" click:
  - Set URL to default: `http://localhost:11434`
  - Clear model selection
- On "Refresh" click:
  - Validate URL is set
  - Create temporary `OllamaWingman` instance with URL
  - Call `listModels()` to fetch available models from ollama server
  - Populate `llmModelCombo` with results
  - Handle errors gracefully (show error message if server unreachable)
  - Note: ollama has `listModels()` implementation that calls `/api/tags` endpoint
- On "Probe" click:
  - Validate URL is set
  - Validate model is set (typed or selected)
  - Call `config.probeOllamaProvider(url, model, errorMsg)`
  - Show success/error message
- On "Add" click:
  - Validate inputs (URL, model)
  - Create `LlmProviderConfig` with user inputs
  - Generate unique ID (e.g., "ollama-{timestamp}")
  - Set display name (e.g., "ollama {model} @ {host}")
  - Call `config.addLlmProvider(providerConfig)`
  - Close dialog
- On "Cancel" click: close dialog
- Model combo is **editable**: user can type custom model name or select from dropdown
  - Implementation: `llmModelCombo->setEditable(true);`
  - Get value: `llmModelCombo->currentText().toStdString()`

## Implementation Details

### Existing Implementation Review

**ollama Wingman** (`lib/src/mind/ai/llm/ollama_wingman.{h,cpp}`):
- ✓ **listModels() implemented**: Calls `/api/tags` endpoint to fetch available models
- Returns `std::vector<std::string>` with model names
- Uses CURL on Linux, Qt Network on macOS/Windows
- Parses JSON response to extract model names from `models[].name` field
- Note: Implementation has a bug on line 153 - uses `llmModel` instead of `llmModelName`

**OpenAI Wingman** (`lib/src/mind/ai/llm/openai_wingman.{h,cpp}`):
- ✓ **listModels() implemented**: Currently returns hardcoded models
- Returns `std::vector<std::string>` with "gpt-3.5-turbo" and "gpt-4"
- TODO comment suggests implementing API call to fetch models
- OpenAI API supports listing models via `/v1/models` endpoint
- Should be enhanced to fetch models dynamically from API

### File Structure

New files to create:

```
app/src/qt/dialogs/
├── add_llm_provider_dialog.h
├── add_llm_provider_dialog.cpp
├── openai_config_dialog.h
├── openai_config_dialog.cpp
├── ollama_config_dialog.h
└── ollama_config_dialog.cpp

lib/src/persistence/
└── llm_provider_configuration_representation.h (if needed for serialization)
```

Modified files:

```
app/src/qt/dialogs/
├── configuration_dialog.h    (add Wingman2Tab class)
└── configuration_dialog.cpp  (implement Wingman2Tab)

lib/src/config/
├── configuration.h           (add LlmProviderConfig struct and methods)
└── configuration.cpp         (implement provider management)

lib/src/mind/ai/llm/
├── ollama_wingman.cpp        (fix bug on line 153: llmModel -> llmModelName)
└── openai_wingman.cpp        (enhance listModels() to call OpenAI API)

lib/src/representations/markdown/
└── markdown_configuration_representation.cpp  (persist LlmProviderConfig)
```

### Configuration Persistence

Extend `MarkdownConfigurationRepresentation` class:

```cpp
// In save() method, add:
void MarkdownConfigurationRepresentation::save(const Configuration& config) {
    // ... existing code ...
    
    // Wingman2 LLM Providers section
    file << endl << "## Wingman2 LLM Providers" << endl << endl;
    
    if (config.getActiveLlmProvider()) {
        file << "Active Provider: " << config.getActiveLlmProvider()->id << endl << endl;
    }
    
    for (const auto& provider : config.getLlmProviders()) {
        file << "### Provider: " << provider.id << endl;
        file << "- Type: " << wingmanProviderToString(provider.providerType) << endl;
        file << "- Display Name: " << provider.displayName << endl;
        file << "- Model: " << provider.llmModel << endl;
        
        if (provider.providerType == WINGMAN_PROVIDER_OPENAI) {
            // Don't save API key in plain text - reference env var
            if (!provider.apiKey.empty()) {
                file << "- API Key: <configured>" << endl;
            }
        } else if (provider.providerType == WINGMAN_PROVIDER_OLLAMA) {
            file << "- URL: " << provider.url << endl;
        }
        
        file << "- Valid: " << (provider.isValid ? "true" : "false") << endl;
        file << endl;
    }
}

// In load() method, add parsing for Wingman2 section
void MarkdownConfigurationRepresentation::load(Configuration& config) {
    // ... existing code ...
    
    if (line.find("## Wingman2 LLM Providers") != string::npos) {
        // Parse provider configurations
        // Implementation details...
    }
}
```

### Provider Probe Implementation

```cpp
bool Configuration::probeOpenAiProvider(
    const string& apiKey, 
    const string& model, 
    string& errorMessage) 
{
    try {
        OpenAiWingman testWingman;
        // Set API key temporarily
        string originalKey = wingmanOpenAiApiKey;
        wingmanOpenAiApiKey = apiKey;
        
        // Try to list models or send test prompt
        CommandWingmanChat testCommand;
        testCommand.prompt = "test";
        testWingman.chat(testCommand);
        
        // Restore original key
        wingmanOpenAiApiKey = originalKey;
        
        if (testCommand.status == WINGMAN_STATUS_CODE_OK) {
            return true;
        } else {
            errorMessage = testCommand.errorMessage;
            return false;
        }
    } catch (const exception& e) {
        errorMessage = string("Probe failed: ") + e.what();
        return false;
    }
}

bool Configuration::probeOllamaProvider(
    const string& url, 
    const string& model, 
    string& errorMessage) 
{
    try {
        OllamaWingman testWingman(url);
        
        // Try to list models
        vector<string>& models = testWingman.listModels();
        
        if (models.empty()) {
            errorMessage = "No models found on ollama server";
            return false;
        }
        
        // Optionally test chat if model specified
        if (!model.empty()) {
            CommandWingmanChat testCommand;
            testCommand.prompt = "test";
            testWingman.setLlmModel(model);
            testWingman.chat(testCommand);
            
            if (testCommand.status == WINGMAN_STATUS_CODE_ERROR) {
                errorMessage = testCommand.errorMessage;
                return false;
            }
        }
        
        return true;
    } catch (const exception& e) {
        errorMessage = string("Probe failed: ") + e.what();
        return false;
    }
}
```

### OpenAI List Models Enhancement

Enhance `OpenAiWingman::listModels()` to call OpenAI API:

```cpp
// In openai_wingman.h - add new private method:
private:
    void listModelsHttpGet();

// In openai_wingman.cpp - enhance listModels():
std::vector<std::string>& OpenAiWingman::listModels()
{
    llmModels.clear();
    
    // Try to fetch models from OpenAI API
    try {
        listModelsHttpGet();
    } catch (...) {
        MF_DEBUG("OpenAiWingman::listModels() failed to fetch from API, using defaults" << endl);
    }
    
    // If API call failed or returned no models, use defaults
    if (llmModels.empty()) {
        llmModels.push_back(LLM_GPT_35_TURBO);
        llmModels.push_back(LLM_GPT_4);
    }
    
    return llmModels;
}

void OpenAiWingman::listModelsHttpGet()
{
    // OpenAI API endpoint for listing models
    string url = "https://api.openai.com/v1/models";
    
    MF_DEBUG("OpenAiWingman::listModelsHttpGet() url: " << url << endl);
    
#if !defined(__APPLE__) && !defined(_WIN32)
    CURL* curl = curl_easy_init();
    if (!curl) {
        return;
    }
#endif

    string responseString;
    
#if defined(_WIN32) || defined(__APPLE__)
    QNetworkAccessManager networkManager;
    
    QNetworkRequest request(QUrl(QString::fromStdString(url)));
    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json");
    request.setRawHeader(
        "Authorization",
        "Bearer " + QString::fromStdString(config.getWingmanOpenAiApiKey()).toUtf8());
    
    QNetworkReply* reply = networkManager.get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    
    auto error = reply->error();
    if (error != QNetworkReply::NoError) {
        MF_DEBUG("OpenAiWingman::listModelsHttpGet() error: " << reply->errorString().toStdString() << endl);
        reply->deleteLater();
        return;
    }
    
    QByteArray read = reply->readAll();
    responseString = QString{read}.toStdString();
    reply->deleteLater();
#else
    // CURL implementation
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, openaiCurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
    
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(
        headers,
        ("Authorization: Bearer " + config.getWingmanOpenAiApiKey()).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    
    if (res != CURLE_OK) {
        MF_DEBUG("OpenAiWingman::listModelsHttpGet() error: " << curl_easy_strerror(res) << endl);
        return;
    }
#endif

    // Parse JSON response
    /*
    OpenAI /v1/models response example:
    {
        "object": "list",
        "data": [
            {
                "id": "gpt-3.5-turbo",
                "object": "model",
                "created": 1677610602,
                "owned_by": "openai"
            },
            {
                "id": "gpt-4",
                "object": "model",
                "created": 1687882411,
                "owned_by": "openai"
            }
        ]
    }
    */
    
    nlohmann::json httpResponseJson;
    try {
        httpResponseJson = nlohmann::json::parse(responseString);
    } catch (...) {
        MF_DEBUG(
            "Error: unable to parse OpenAI models JSON response:" << endl <<
            "'" << responseString << "'" << endl
        );
        return;
    }
    
    MF_DEBUG(
        "OpenAiWingman::listModelsHttpGet() parsed response:" << endl
        << ">>>"
        << httpResponseJson.dump(4)
        << "<<<"
        << endl);
    
    if (httpResponseJson.contains("data")) {
        for (const auto& item : httpResponseJson["data"].items()) {
            if (item.value().contains("id")) {
                string modelId = item.value()["id"];
                // Filter to only include GPT models (optional)
                if (modelId.find("gpt") != string::npos) {
                    llmModels.push_back(modelId);
                    MF_DEBUG("  Added model: " << modelId << endl);
                }
            }
        }
    }
}
```

### ollama Bug Fix

Fix bug in `ollama_wingman.cpp` line 153:

```cpp
// BEFORE (incorrect):
llmModels.push_back(llmModel);

// AFTER (correct):
llmModels.push_back(llmModelName);
```

### Integration with Existing Wingman

To maintain backward compatibility and integrate with existing Wingman code:

```cpp
// In Mind class initialization
void Mind::initWingman() {
    Configuration& config = Configuration::getInstance();
    
    // Try new Wingman2 configuration first
    LlmProviderConfig* activeProvider = config.getActiveLlmProvider();
    if (activeProvider && activeProvider->isValid) {
        switch (activeProvider->providerType) {
            case WINGMAN_PROVIDER_OPENAI:
                wingman = new OpenAiWingman();
                wingman->setLlmModel(activeProvider->llmModel);
                break;
            case WINGMAN_PROVIDER_OLLAMA:
                wingman = new OllamaWingman(activeProvider->url);
                wingman->setLlmModel(activeProvider->llmModel);
                break;
            default:
                break;
        }
    } else {
        // Fall back to legacy Wingman configuration
        if (config.initWingman()) {
            // Use existing initialization code
        }
    }
}
```

## Testing Strategy

### Unit Tests

Location: `lib/test/src/config/`

**Test File**: `llm_provider_config_test.cpp`

Test cases:

```cpp
TEST(LlmProviderConfigTest, AddProvider) {
    // GIVEN
    Configuration& config = Configuration::getInstance();
    LlmProviderConfig provider;
    provider.id = "test-openai";
    provider.displayName = "Test OpenAI";
    provider.providerType = WINGMAN_PROVIDER_OPENAI;
    provider.apiKey = "test-key";
    provider.llmModel = "gpt-3.5-turbo";
    
    // WHEN
    config.addLlmProvider(provider);
    
    // THEN
    LlmProviderConfig* retrieved = config.getLlmProviderById("test-openai");
    ASSERT_NE(nullptr, retrieved);
    ASSERT_EQ("Test OpenAI", retrieved->displayName);
    ASSERT_EQ(WINGMAN_PROVIDER_OPENAI, retrieved->providerType);
}

TEST(LlmProviderConfigTest, UpdateProvider) {
    // GIVEN
    Configuration& config = Configuration::getInstance();
    LlmProviderConfig provider;
    provider.id = "test-ollama";
    provider.providerType = WINGMAN_PROVIDER_OLLAMA;
    provider.url = "http://localhost:11434";
    config.addLlmProvider(provider);
    
    // WHEN
    provider.llmModel = "llama2";
    config.updateLlmProvider("test-ollama", provider);
    
    // THEN
    LlmProviderConfig* updated = config.getLlmProviderById("test-ollama");
    ASSERT_EQ("llama2", updated->llmModel);
}

TEST(LlmProviderConfigTest, RemoveProvider) {
    // GIVEN
    Configuration& config = Configuration::getInstance();
    LlmProviderConfig provider;
    provider.id = "test-remove";
    config.addLlmProvider(provider);
    
    // WHEN
    config.removeLlmProvider("test-remove");
    
    // THEN
    LlmProviderConfig* retrieved = config.getLlmProviderById("test-remove");
    ASSERT_EQ(nullptr, retrieved);
}

TEST(LlmProviderConfigTest, SetActiveProvider) {
    // GIVEN
    Configuration& config = Configuration::getInstance();
    LlmProviderConfig provider;
    provider.id = "test-active";
    config.addLlmProvider(provider);
    
    // WHEN
    config.setActiveLlmProvider("test-active");
    
    // THEN
    LlmProviderConfig* active = config.getActiveLlmProvider();
    ASSERT_NE(nullptr, active);
    ASSERT_EQ("test-active", active->id);
}

TEST(LlmProviderConfigTest, PersistenceRoundTrip) {
    // GIVEN
    Configuration& config = Configuration::getInstance();
    LlmProviderConfig provider;
    provider.id = "persist-test";
    provider.displayName = "Persistence Test";
    provider.providerType = WINGMAN_PROVIDER_OPENAI;
    provider.llmModel = "gpt-4";
    config.addLlmProvider(provider);
    config.setActiveLlmProvider("persist-test");
    
    // WHEN
    string configPath = "/tmp/test-wingman2-config.md";
    MarkdownConfigurationRepresentation representation(configPath);
    representation.save(config);
    
    Configuration& loadedConfig = Configuration::getInstance();
    loadedConfig.clear();
    representation.load(loadedConfig);
    
    // THEN
    LlmProviderConfig* loaded = loadedConfig.getLlmProviderById("persist-test");
    ASSERT_NE(nullptr, loaded);
    ASSERT_EQ("Persistence Test", loaded->displayName);
    ASSERT_EQ(WINGMAN_PROVIDER_OPENAI, loaded->providerType);
    ASSERT_EQ("gpt-4", loaded->llmModel);
    
    LlmProviderConfig* activeLoaded = loadedConfig.getActiveLlmProvider();
    ASSERT_NE(nullptr, activeLoaded);
    ASSERT_EQ("persist-test", activeLoaded->id);
}
```

### Integration Tests

**Test File**: `lib/test/src/mind/wingman_integration_test.cpp`

Test cases:

```cpp
TEST(WingmanIntegrationTest, UseConfiguredProvider) {
    // GIVEN - Configure OpenAI provider
    Configuration& config = Configuration::getInstance();
    LlmProviderConfig provider;
    provider.id = "integration-test";
    provider.providerType = WINGMAN_PROVIDER_OPENAI;
    provider.apiKey = getenv(ENV_VAR_OPENAI_API_KEY) ?: "test-key";
    provider.llmModel = "gpt-3.5-turbo";
    provider.isValid = true;
    config.addLlmProvider(provider);
    config.setActiveLlmProvider("integration-test");
    
    // WHEN - Initialize Mind with configured provider
    Mind mind(config);
    mind.initWingman();
    
    // THEN - Wingman should be initialized with correct provider
    Wingman* wingman = mind.getWingman();
    ASSERT_NE(nullptr, wingman);
    ASSERT_EQ("gpt-3.5-turbo", wingman->getLlmModel());
}

TEST(WingmanIntegrationTest, FallbackToLegacyConfig) {
    // GIVEN - No Wingman2 providers configured, but legacy config exists
    Configuration& config = Configuration::getInstance();
    config.setWingmanLlmProvider(WINGMAN_PROVIDER_OPENAI);
    config.setWingmanOpenAiApiKey("legacy-key");
    config.setWingmanOpenAiLlm("gpt-4");
    
    // WHEN - Initialize Mind
    Mind mind(config);
    mind.initWingman();
    
    // THEN - Wingman should be initialized using legacy configuration
    Wingman* wingman = mind.getWingman();
    ASSERT_NE(nullptr, wingman);
    ASSERT_EQ("gpt-4", wingman->getLlmModel());
}
```

### UI Tests

Manual test scenarios:

1. **Add OpenAI Provider**
   - Open Preferences → Wingman2 tab
   - Click "Add LLM Provider"
   - Select "OpenAI" and click "Next"
   - Enter valid API key
   - Select model
   - Click "Probe" - should show success
   - Click "Add" - provider should appear in dropdown

2. **Add ollama Provider**
   - Open Preferences → Wingman2 tab
   - Click "Add LLM Provider"
   - Select "ollama" and click "Next"
   - Enter ollama URL
   - Click "Refresh" - should populate models
   - Select model
   - Click "Probe" - should show success
   - Click "Add" - provider should appear in dropdown

3. **Switch Between Providers**
   - Configure multiple providers
   - Select different provider from dropdown
   - Click "Test Connection" - should validate current selection
   - Save configuration
   - Restart MindForger
   - Verify active provider is preserved

4. **Edit Provider**
   - Select a provider
   - Click "Edit"
   - Modify configuration
   - Click "Probe" to validate
   - Save changes

5. **Remove Provider**
   - Select a provider
   - Click "Remove"
   - Confirm deletion
   - Verify provider is removed from dropdown

## Documentation

### User Documentation

Update `doc/user-guide.md` with new section:

```markdown
## Configuring Wingman LLM Providers (Wingman2)

MindForger supports multiple Large Language Model providers through the Wingman2 configuration interface.

### Adding an LLM Provider

1. Open **Workspace** → **Preferences**
2. Navigate to the **Wingman2** tab
3. Click **Add LLM Provider**
4. Select your provider type (OpenAI or ollama)
5. Configure provider-specific settings
6. Click **Probe** to test the connection
7. Click **Add** to save the configuration

### OpenAI Configuration

To configure OpenAI as your LLM provider:

1. Obtain an API key from [OpenAI](https://platform.openai.com/api-keys)
2. In the OpenAI configuration dialog:
   - Enter your API key (or set the `MINDFORGER_OPENAI_API_KEY` environment variable)
   - Click **Refresh** to fetch available models from OpenAI API
   - Select your preferred model from the dropdown, or type a custom model name (e.g., gpt-3.5-turbo, gpt-4, gpt-4-turbo)
   - Click **Probe** to validate the configuration
3. Click **Add** to save

**Tip**: Use the **Reset** button to restore default settings (clears API key, sets default model).

**Note**: Your data will be sent to OpenAI's servers when using Wingman with this provider.

### ollama Configuration

To configure ollama as your LLM provider:

1. Install and start [ollama](https://ollama.com) on your machine or server
2. In the ollama configuration dialog:
   - Enter the ollama server URL (or click **Reset** to use default: `http://localhost:11434`)
   - Click **Refresh** to fetch available models from the ollama server
   - Select your preferred model from the dropdown, or type a custom model name (e.g., llama2, mistral, phi)
   - Click **Probe** to validate the configuration
3. Click **Add** to save

**Tip**: Use the **Reset** button to restore default URL and clear model selection.

**Note**: ollama runs locally, so your data stays on your machine.

### Managing Providers

- **Switch providers**: Select a different provider from the dropdown
- **Edit provider**: Click **Edit** to modify configuration
- **Remove provider**: Click **Remove** to delete a configuration
- **Test connection**: Click **Test Connection** to validate the current provider
- **Custom model names**: When adding or editing a provider, you can type any model name in the model field, not just select from the predefined list. This is useful for:
  - New models released by providers (e.g., gpt-4-turbo, gpt-4-vision)
  - Custom ollama models you've pulled locally
  - Model variants with specific parameters (e.g., llama2:13b, mistral:latest)

### Migration from Legacy Wingman Configuration

If you have configured Wingman using the original Wingman tab, your configuration will be automatically migrated to Wingman2 on first use. The original Wingman tab remains available for reference and backward compatibility.
```

### Developer Documentation

Update Doxygen comments in header files:

```cpp
/**
 * @brief LLM Provider Configuration
 * 
 * Represents configuration for a single Large Language Model provider.
 * Supports OpenAI and ollama providers with provider-specific fields.
 * 
 * @see Configuration::addLlmProvider
 * @see Configuration::getLlmProviders
 */
struct LlmProviderConfig {
    // ... fields ...
};

/**
 * @brief Add a new LLM provider configuration
 * 
 * Adds a new provider to the list of configured providers.
 * The provider ID must be unique.
 * 
 * @param provider The provider configuration to add
 * @throws std::invalid_argument if provider with same ID exists
 */
void addLlmProvider(const LlmProviderConfig& provider);
```

## Implementation Checklist

### Phase 1: Data Model & Configuration
- [ ] Define `LlmProviderConfig` struct in `configuration.h`
- [ ] Add `llmProviders` vector and `activeLlmProviderId` fields to `Configuration`
- [ ] Implement `addLlmProvider()` method
- [ ] Implement `updateLlmProvider()` method
- [ ] Implement `removeLlmProvider()` method
- [ ] Implement `getLlmProviderById()` method
- [ ] Implement `getLlmProviders()` method
- [ ] Implement `setActiveLlmProvider()` method
- [ ] Implement `getActiveLlmProvider()` method
- [ ] Implement `probeOpenAiProvider()` method
- [ ] Implement `probeOllamaProvider()` method
- [ ] Implement `migrateFromLegacyWingmanConfig()` method

### Phase 2: Configuration Persistence
- [ ] Extend `MarkdownConfigurationRepresentation::save()` to persist LLM providers
- [ ] Extend `MarkdownConfigurationRepresentation::load()` to load LLM providers
- [ ] Add helper function `wingmanProviderToString()`
- [ ] Add helper function `stringToWingmanProvider()`
- [ ] Test configuration persistence round-trip

### Phase 3: UI - Main Wingman2 Tab
- [ ] Create `Wingman2Tab` class in `configuration_dialog.h`
- [ ] Implement `Wingman2Tab` constructor with layout
- [ ] Implement `Wingman2Tab::refresh()` method
- [ ] Implement `Wingman2Tab::save()` method
- [ ] Add provider dropdown population logic
- [ ] Add provider details display
- [ ] Implement "Add Provider" button handler
- [ ] Implement "Edit" button handler
- [ ] Implement "Remove" button handler
- [ ] Implement "Test Connection" button handler
- [ ] Add Wingman2 tab to `ConfigurationDialog` tab widget

### Phase 4: UI - Add LLM Provider Dialog
- [ ] Create `add_llm_provider_dialog.h` header file
- [ ] Create `add_llm_provider_dialog.cpp` implementation
- [ ] Implement `AddLlmProviderDialog` class constructor
- [ ] Add provider type combo box (OpenAI, ollama)
- [ ] Implement "Next" button handler
- [ ] Implement "Cancel" button handler
- [ ] Add dialog styling and layout

### Phase 5: UI - OpenAI Config Dialog
- [ ] Create `openai_config_dialog.h` header file
- [ ] Create `openai_config_dialog.cpp` implementation
- [ ] Implement `OpenAiConfigDialog` class constructor
- [ ] Add API key input field (masked)
- [ ] Add "Reset" button for API key (reset to defaults)
- [ ] Add environment variable info label
- [ ] Add **editable** LLM model combo box (user can type or select)
- [ ] Populate model dropdown with default models
- [ ] Add "Refresh" button for fetching models from OpenAI API
- [ ] Implement "Refresh" button handler (call OpenAI listModels())
- [ ] Implement "Reset" button handler (clear API key, set default model)
- [ ] Implement "Probe" button handler
- [ ] Implement "Add" button handler
- [ ] Implement "Cancel" button handler
- [ ] Add input validation (API key required, model required)

### Phase 6: UI - ollama Config Dialog
- [ ] Create `ollama_config_dialog.h` header file
- [ ] Create `ollama_config_dialog.cpp` implementation
- [ ] Implement `OllamaConfigDialog` class constructor
- [ ] Add URL input field
- [ ] Add "Reset" button for URL (reset to default: http://localhost:11434)
- [ ] Add **editable** LLM model combo box (user can type or select)
- [ ] Add "Refresh" button for fetching models from ollama server
- [ ] Implement "Refresh" button handler (call ollama listModels())
- [ ] Implement "Reset" button handler (set default URL, clear model)
- [ ] Implement "Probe" button handler
- [ ] Implement "Add" button handler
- [ ] Implement "Cancel" button handler
- [ ] Add input validation (URL required, model required)

### Phase 7: Integration with Mind
- [ ] Update `Mind::initWingman()` to check Wingman2 config first
- [ ] Add fallback to legacy Wingman configuration
- [ ] Update Wingman dialog initialization to use active provider
- [ ] Test Wingman functionality with configured providers
- [ ] Verify provider switching works at runtime

### Phase 8: Bug Fixes & Enhancements
- [ ] **FIX** ollama_wingman.cpp line 153: change `llmModels.push_back(llmModel)` to `llmModels.push_back(llmModelName)`
- [ ] **ENHANCE** openai_wingman.cpp `listModels()`: implement OpenAI API call to `/v1/models`
- [ ] Verify ollama listModels() works correctly after bug fix
- [ ] Verify OpenAI listModels() returns actual models from API

### Phase 9: Testing
- [ ] Write unit tests for `LlmProviderConfig` operations
- [ ] Write unit tests for provider persistence
- [ ] Write unit tests for probe functionality
- [ ] Write unit tests for editable combo boxes (custom model names)
- [ ] Write integration tests for Mind-Wingman initialization
- [ ] Write integration tests for provider fallback
- [ ] Perform manual UI testing for all dialogs
- [ ] Test "Reset" button functionality in both dialogs
- [ ] Test "Refresh" button functionality in both dialogs
- [ ] Test typing custom model names in editable combo boxes
- [ ] Test configuration migration from legacy Wingman
- [ ] Test error handling and edge cases

### Phase 10: Documentation
- [ ] Update user documentation with Wingman2 configuration guide
- [ ] Document editable combo box feature (type custom model names)
- [ ] Document "Reset" and "Refresh" button functionality
- [ ] Add Doxygen comments to new classes and methods
- [ ] Update `CHANGELOG` with new feature description
- [ ] Update `CHANGELOG` with bug fix for ollama listModels()
- [ ] Update `CHANGELOG` with enhancement for OpenAI listModels()
- [ ] Create screenshots for user documentation
- [ ] Update any affected design documents

### Phase 11: Code Quality & Review
- [ ] Run code linter on all new files
- [ ] Verify code follows MindForger coding conventions
- [ ] Review for memory leaks (Qt parent-child hierarchy)
- [ ] Review for proper error handling
- [ ] Verify cross-platform compatibility (Linux, macOS, Windows)
- [ ] Check for proper use of Qt translation strings (tr())
- [ ] Verify no hardcoded strings in UI
- [ ] Code review with team

### Phase 12: Build & CI
- [ ] Update qmake project files to include new source files
- [ ] Update `build/Makefile` if needed
- [ ] Verify builds on Linux
- [ ] Verify builds on macOS (GitHub Actions)
- [ ] Verify builds on Windows (AppVeyor)
- [ ] Fix any compilation warnings or errors

### Phase 13: Final Testing & Release
- [ ] Perform end-to-end testing with real OpenAI account
- [ ] Test OpenAI model refresh with real API
- [ ] Test typing custom OpenAI model names (e.g., "gpt-4-turbo")
- [ ] Perform end-to-end testing with local ollama server
- [ ] Test ollama model refresh from local server
- [ ] Test typing custom ollama model names (e.g., "mistral:latest")
- [ ] Test "Reset" button restores defaults correctly
- [ ] Test on clean installation (no existing config)
- [ ] Test with existing Wingman configuration (migration)
- [ ] Test configuration save/load across restarts
- [ ] Verify backward compatibility
- [ ] Create release branch following naming convention
- [ ] Update version numbers in all required files
- [ ] Create Git tag following convention (vMAJOR.MINOR.PATCH)

## Security Considerations

### API Key Storage

- **OpenAI API Keys**: 
  - Should NOT be stored in plain text in configuration file
  - Prefer environment variable `MINDFORGER_OPENAI_API_KEY`
  - If stored in config, use basic obfuscation (not encryption, as key is in same file)
  - Warn user about security implications

### Data Privacy

- **OpenAI Provider**:
  - Display warning that data is sent to OpenAI servers
  - Include in dialog and documentation
  - User must explicitly acknowledge

- **ollama Provider**:
  - Highlight that data stays local
  - Mention in documentation as privacy-friendly option

### Input Validation

- Validate all user inputs before storage
- Sanitize URLs for ollama provider
- Check for valid API key format for OpenAI
- Prevent injection attacks in configuration file

### Network Security

- Use HTTPS for OpenAI API calls
- Validate SSL certificates
- Handle network errors gracefully
- Implement timeout for probe operations

## Backward Compatibility

### Migration Strategy

When user opens Wingman2 tab for the first time:

1. Check if legacy Wingman configuration exists
2. If yes and no Wingman2 providers configured:
   - Create equivalent `LlmProviderConfig` from legacy config
   - Set as active provider
   - Notify user of migration
3. Keep legacy Wingman tab functional
4. Both configurations can coexist

### Deprecation Plan

- Wingman2 is the recommended configuration method
- Legacy Wingman tab remains for backward compatibility
- In future major version, consider removing legacy tab
- Provide clear migration path in documentation

## Future Enhancements

Potential future improvements (not in scope for this design):

1. **Additional Providers**
   - Support for Anthropic Claude
   - Support for Google Gemini
   - Support for Azure OpenAI

2. **Advanced Features**
   - Provider-specific prompt templates
   - Token usage tracking and limits
   - Cost estimation
   - Provider performance metrics

3. **UI Improvements**
   - Import/export provider configurations
   - Provider presets/templates
   - Bulk provider management

4. **Security**
   - Encrypted API key storage
   - Integration with system keychain
   - OAuth support for providers that offer it

## Risk Analysis

### Technical Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| Qt version compatibility issues | Medium | Low | Test on all supported Qt versions |
| Configuration file corruption | High | Low | Implement robust parsing with error recovery |
| Memory leaks in Qt UI | Medium | Medium | Follow Qt parent-child hierarchy pattern |
| Network timeouts during probe | Low | High | Implement proper timeout handling |
| API key security | High | Medium | Use environment variables, warn users |

### User Experience Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| Configuration too complex | Medium | Medium | Clear documentation, intuitive UI |
| Migration from legacy confusing | Medium | Low | Automatic migration, clear messaging |
| Provider switching not obvious | Low | Low | Prominent dropdown, clear labels |

## Success Criteria

The implementation will be considered successful when:

1. ✓ Users can add, edit, and remove LLM providers through the UI
2. ✓ Multiple providers can be configured and switched between
3. ✓ Probe functionality validates provider configurations
4. ✓ Configuration persists across application restarts
5. ✓ Legacy Wingman configuration is automatically migrated
6. ✓ All tests pass (unit, integration, manual)
7. ✓ Documentation is complete and accurate
8. ✓ Code builds successfully on all platforms
9. ✓ No memory leaks or crashes
10. ✓ User feedback is positive

## Glossary

- **LLM**: Large Language Model
- **Wingman**: MindForger's AI assistant feature
- **Wingman2**: New improved LLM configuration system
- **Provider**: Service that hosts LLM models (OpenAI, ollama)
- **Probe**: Test/validate a provider configuration
- **ollama**: Open-source LLM runtime that runs locally
- **Configuration Dialog**: MindForger's Preferences/Settings window

## References

- Existing Wingman implementation: `lib/src/mind/ai/llm/`
- OpenAI API documentation: https://platform.openai.com/docs/api-reference
- ollama documentation: https://ollama.com
- Qt documentation: https://doc.qt.io/
- MindForger repository: https://github.com/dvorka/mindforger

---

**Document Version**: 1.0  
**Last Updated**: 2026-02-08  
**Status**: READY FOR IMPLEMENTATION
