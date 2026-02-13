#include "SceneEnd.h"
#include "Game.h"
#include "SceneTitle.h"
#include "SceneMain.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <SDL_log.h>

// 修复 CreateDirectoryA 未定义问题：包含Windows头文件
#ifdef _WIN32
#include <windows.h>
#endif

// 得分榜文件路径
const std::string SCOREBOARD_FILE = "assets/data/scoreboard.txt";

SceneEnd::SceneEnd() : game_(Game::getInstance()) {
    // 初始化场景切换相关变量（禁用自动跳转）
    sceneSwitchDelay_ = 3.0f; 
    sceneSwitchTimer_ = 0.0f;
    sceneSwitchTriggered_ = false;
    
    // 初始化玩家名字输入相关变量
    playerName_ = "";
    isNameInputActive_ = true;  // 默认显示名字输入界面
    cursorPosition_ = 0;
    cursorBlinkTimer_ = 0.0f;
    cursorVisible_ = true;
    
    // 初始化得分榜相关变量
    maxHighScores_ = 10; // 最多显示10个高分记录
    isScoreboardVisible_ = false;
    
    // 初始化字体
    titleFont_ = nullptr;
    textFont_ = nullptr;
    scoreFont_ = nullptr;
}

SceneEnd::~SceneEnd() {
    clean();
}

void SceneEnd::init() {
    // 加载支持中文的字体（替换为你的中文字体路径）
    titleFont_ = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 48);
    if(titleFont_ == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load title font! SDL_ttf Error: %s\n", TTF_GetError());
    }
    
    textFont_ = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);
    if(textFont_ == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load text font! SDL_ttf Error: %s\n", TTF_GetError());
    }
    
    scoreFont_ = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 20);
    if(scoreFont_ == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load score font! SDL_ttf Error: %s\n", TTF_GetError());
    }

    // 加载得分榜
    loadHighScores();
    
    // 开始文本输入（支持中文输入）
    SDL_StartTextInput();
}

void SceneEnd::update(float deltaTime) {
    // 完全移除自动跳转逻辑，只保留光标闪烁
    cursorBlinkTimer_ += deltaTime;
    if (cursorBlinkTimer_ >= 0.5f) {
        cursorVisible_ = !cursorVisible_;
        cursorBlinkTimer_ = 0.0f;
    }
}

void SceneEnd::render() {
    // 清空屏幕为黑色背景（匹配截图）
    SDL_SetRenderDrawColor(game_.getRenderer(), 0, 0, 0, 255);
    SDL_RenderClear(game_.getRenderer());
    
    // 如果名字输入处于活动状态，渲染名字输入界面（匹配截图）
    if (isNameInputActive_) {
        // 1. 渲染"你的得分是：XXX"
        std::string scoreText = "Your Score：" + std::to_string(game_.getScore());
        SDL_Color whiteColor = {255, 255, 255, 255};
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(textFont_, scoreText.c_str(), whiteColor);
        if (scoreSurface != nullptr) {
            SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), scoreSurface);
            int scoreWidth = scoreSurface->w;
            int scoreHeight = scoreSurface->h;
            SDL_Rect scoreRect = {
                (game_.getWindowWidth() - scoreWidth) / 2,
                80,
                scoreWidth,
                scoreHeight
            };
            SDL_RenderCopy(game_.getRenderer(), scoreTexture, nullptr, &scoreRect);
            SDL_DestroyTexture(scoreTexture);
            SDL_FreeSurface(scoreSurface);
        }

        // 2. 渲染"Game Over"标题
        const char* gameOverText = "Game Over";
        SDL_Surface* gameOverSurface = TTF_RenderText_Solid(titleFont_, gameOverText, whiteColor);
        if (gameOverSurface != nullptr) {
            SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), gameOverSurface);
            int gameOverWidth = gameOverSurface->w;
            int gameOverHeight = gameOverSurface->h;
            SDL_Rect gameOverRect = {
                (game_.getWindowWidth() - gameOverWidth) / 2,
                180,
                gameOverWidth,
                gameOverHeight
            };
            SDL_RenderCopy(game_.getRenderer(), gameOverTexture, nullptr, &gameOverRect);
            SDL_DestroyTexture(gameOverTexture);
            SDL_FreeSurface(gameOverSurface);
        }

        // 3. 渲染"请输入你的名字，按回车键确认："
        const char* promptText = "Enter your name and press Enter:";
        SDL_Surface* promptSurface = TTF_RenderText_Solid(textFont_, promptText, whiteColor);
        if (promptSurface != nullptr) {
            SDL_Texture* promptTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), promptSurface);
            int promptWidth = promptSurface->w;
            int promptHeight = promptSurface->h;
            SDL_Rect promptRect = {
                (game_.getWindowWidth() - promptWidth) / 2,
                320,
                promptWidth,
                promptHeight
            };
            SDL_RenderCopy(game_.getRenderer(), promptTexture, nullptr, &promptRect);
            SDL_DestroyTexture(promptTexture);
            SDL_FreeSurface(promptSurface);
        }

        // 4. 渲染玩家名字输入框 + 闪烁光标
        const char* playerNameText = playerName_.empty() ? "" : playerName_.c_str();
        SDL_Surface* nameSurface = TTF_RenderText_Solid(textFont_, playerNameText, whiteColor);
        if (nameSurface != nullptr) {
            SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), nameSurface);
            int nameWidth = nameSurface->w;
            int nameHeight = nameSurface->h;
            SDL_Rect nameRect = {
                (game_.getWindowWidth() - nameWidth) / 2,
                380,
                nameWidth,
                nameHeight
            };
            SDL_RenderCopy(game_.getRenderer(), nameTexture, nullptr, &nameRect);

            // 渲染闪烁光标
            if (cursorVisible_) {
                SDL_SetRenderDrawColor(game_.getRenderer(), 255, 255, 255, 255);
                SDL_Rect cursorRect = {
                    nameRect.x + nameWidth + 2,
                    nameRect.y,
                    2,
                    nameHeight
                };
                SDL_RenderFillRect(game_.getRenderer(), &cursorRect);
            }

            SDL_DestroyTexture(nameTexture);
            SDL_FreeSurface(nameSurface);
        }

        // 5. 渲染底部"SDL"标识
        const char* sdlText = "SDL";
        SDL_Surface* sdlSurface = TTF_RenderText_Solid(textFont_, sdlText, whiteColor);
        if (sdlSurface != nullptr) {
            SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), sdlSurface);
            int sdlWidth = sdlSurface->w;
            int sdlHeight = sdlSurface->h;
            SDL_Rect sdlRect = {
                (game_.getWindowWidth() - sdlWidth) / 2,
                game_.getWindowHeight() - 80,
                sdlWidth,
                sdlHeight
            };
            SDL_RenderCopy(game_.getRenderer(), sdlTexture, nullptr, &sdlRect);
            SDL_DestroyTexture(sdlTexture);
            SDL_FreeSurface(sdlSurface);
        }
    }
    
    // 如果得分榜可见，渲染得分榜（匹配截图）
    if (isScoreboardVisible_) {
        // 1. 渲染"得分榜"标题
        const char* titleText = "Scoreboard";
        SDL_Color goldColor = {255, 215, 0, 255}; // 金色
        SDL_Surface* titleSurface = TTF_RenderText_Solid(titleFont_, titleText, goldColor);
        if (titleSurface != nullptr) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), titleSurface);
            int titleWidth = titleSurface->w;
            int titleHeight = titleSurface->h;
            SDL_Rect titleRect = {
                (game_.getWindowWidth() - titleWidth) / 2,
                60,
                titleWidth,
                titleHeight
            };
            SDL_RenderCopy(game_.getRenderer(), titleTexture, nullptr, &titleRect);
            SDL_DestroyTexture(titleTexture);
            SDL_FreeSurface(titleSurface);
        }

        // 2. 渲染得分记录（最多显示8条，匹配截图）
        int startY = 160;
        int lineHeight = 40;
        SDL_Color whiteColor = {255, 255, 255, 255};
        
        for (size_t i = 0; i < highScores_.size() && i < 8; ++i) {
            // 左对齐：排名 + 名字
            std::string leftText = std::to_string(i + 1) + ". " + highScores_[i].playerName;
            SDL_Surface* leftSurface = TTF_RenderText_Solid(scoreFont_, leftText.c_str(), whiteColor);
            if (leftSurface != nullptr) {
                SDL_Texture* leftTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), leftSurface);
                int leftWidth = leftSurface->w;
                int leftHeight = leftSurface->h;
                SDL_Rect leftRect = {
                    (game_.getWindowWidth() / 2) - 180,
                    startY + static_cast<int>(i) * lineHeight,
                    leftWidth,
                    leftHeight
                };
                SDL_RenderCopy(game_.getRenderer(), leftTexture, nullptr, &leftRect);
                SDL_DestroyTexture(leftTexture);
                SDL_FreeSurface(leftSurface);
            }

            // 右对齐：分数
            std::string rightText = std::to_string(highScores_[i].score);
            SDL_Surface* rightSurface = TTF_RenderText_Solid(scoreFont_, rightText.c_str(), whiteColor);
            if (rightSurface != nullptr) {
                SDL_Texture* rightTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), rightSurface);
                int rightWidth = rightSurface->w;
                int rightHeight = rightSurface->h;
                SDL_Rect rightRect = {
                    (game_.getWindowWidth() / 2) + 180 - rightWidth,
                    startY + static_cast<int>(i) * lineHeight,
                    rightWidth,
                    rightHeight
                };
                SDL_RenderCopy(game_.getRenderer(), rightTexture, nullptr, &rightRect);
                SDL_DestroyTexture(rightTexture);
                SDL_FreeSurface(rightSurface);
            }
        }

        // 3. 渲染底部提示：按 J 键重新开始游戏
        const char* hintText = "Press the J key to restart the game";
        SDL_Color grayColor = {200, 200, 200, 255};
        SDL_Surface* hintSurface = TTF_RenderText_Solid(textFont_, hintText, grayColor);
        if (hintSurface != nullptr) {
            SDL_Texture* hintTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), hintSurface);
            int hintWidth = hintSurface->w;
            int hintHeight = hintSurface->h;
            SDL_Rect hintRect = {
                (game_.getWindowWidth() - hintWidth) / 2,
                game_.getWindowHeight() - 80,
                hintWidth,
                hintHeight
            };
            SDL_RenderCopy(game_.getRenderer(), hintTexture, nullptr, &hintRect);
            SDL_DestroyTexture(hintTexture);
            SDL_FreeSurface(hintSurface);
        }
    }
    
    // 刷新渲染器
    SDL_RenderPresent(game_.getRenderer());
}

void SceneEnd::clean() {
    // 停止文本输入
    SDL_StopTextInput();
    
    // 释放字体资源
    if (titleFont_ != nullptr) {
        TTF_CloseFont(titleFont_);
        titleFont_ = nullptr;
    }
    
    if (textFont_ != nullptr) {
        TTF_CloseFont(textFont_);
        textFont_ = nullptr;
    }
    
    if (scoreFont_ != nullptr) {
        TTF_CloseFont(scoreFont_);
        scoreFont_ = nullptr;
    }
}

void SceneEnd::handleEvent(SDL_Event* event) {
    // 处理退出事件
    if (event->type == SDL_QUIT) {
        game_.quit();
    }
    
    // 处理键盘事件
    if (event->type == SDL_KEYDOWN) {
        // 如果名字输入处于活动状态，处理文本输入
        if (isNameInputActive_) {
            // 处理回车键，完成名字输入
            if (event->key.keysym.sym == SDLK_RETURN) {
                isNameInputActive_ = false;
                isScoreboardVisible_ = true;
                addScoreToLeaderboard(game_.getScore());
            }
            // 处理退格键
            else if (event->key.keysym.sym == SDLK_BACKSPACE) {
                handleBackspace();
            }
        }
        // 如果得分榜可见，处理按键事件
        else if (isScoreboardVisible_) {
            // 处理回车键，返回标题场景
            if (event->key.keysym.sym == SDLK_RETURN) {
                switchToTitleScene();
            }
            // 修复 SDLK_J 未定义：只检测小写j，兼容大小写输入
            else if (event->key.keysym.sym == SDLK_j) {
                game_.changeScene(new SceneMain());
            }
        }
    }
    
    // 处理文本输入事件（支持中文）
    if (event->type == SDL_TEXTINPUT && isNameInputActive_) {
        handleTextInput(event);
    }
}

void SceneEnd::saveHighScores() {
    // 确保目录存在
    std::string dir = "assets/data";
#ifdef _WIN32
    CreateDirectoryA(dir.c_str(), NULL);
#else
    mkdir(dir.c_str(), 0755);
#endif

    // 打开文件进行写入
    std::ofstream outFile(SCOREBOARD_FILE);
    if (!outFile.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open scoreboard file for writing: %s", SCOREBOARD_FILE.c_str());
        return;
    }
    
    // 写入得分记录
    for (size_t i = 0; i < highScores_.size(); ++i) {
        outFile << highScores_[i].playerName << "," 
                << highScores_[i].score << "," 
                << highScores_[i].date << std::endl;
    }
    
    outFile.close();
}

void SceneEnd::loadHighScores() {
    // 清空当前得分榜
    highScores_.clear();
    
    // 打开文件进行读取
    std::ifstream inFile(SCOREBOARD_FILE);
    if (!inFile.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open scoreboard file for reading: %s", SCOREBOARD_FILE.c_str());
        return;
    }
    
    // 读取得分记录
    std::string line;
    int lineNumber = 0;
    while (std::getline(inFile, line)) {
        lineNumber++;
        std::istringstream iss(line);
        std::string playerName, scoreStr, date;
        
        if (std::getline(iss, playerName, ',') && 
            std::getline(iss, scoreStr, ',') && 
            std::getline(iss, date, ',')) {
            
            ScoreRecord record;
            try {
                record.score = std::stoi(scoreStr);
            } catch (const std::exception& e) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid score format at line %d: %s", lineNumber, e.what());
                continue; // 跳过无效记录
            }
            record.date = date;
            
            highScores_.push_back(record);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid format at line %d", lineNumber);
        }
    }
    
    inFile.close();
    
    // 使用更高效的排序算法
    std::sort(highScores_.begin(), highScores_.end(), 
              [](const ScoreRecord& a,const ScoreRecord& b) {
                  return a.score > b.score;
              });
    
    // 限制得分榜记录数量
    if (highScores_.size() > static_cast<size_t>(maxHighScores_)) {
        highScores_.resize(maxHighScores_);
    }
}

void SceneEnd::addScoreToLeaderboard(int score) {
    // 获取当前日期
    time_t now = time(nullptr);
    struct tm timeinfo{};
#ifdef _WIN32
    localtime_s(&timeinfo, &now);
#else
    localtime_r(&now, &timeinfo);
#endif
    
    char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);
    
    // 创建新的得分记录
    ScoreRecord newRecord;
    newRecord.playerName = playerName_.empty() ? "Anonymous" : playerName_;
    newRecord.score = score;
    newRecord.date = dateStr;
    
    // 添加到得分榜
    highScores_.push_back(newRecord);
    
    // 使用更高效的排序算法
    std::sort(highScores_.begin(), highScores_.end(), 
              [](const ScoreRecord& a,const ScoreRecord& b) {
                  return a.score > b.score;
              });
    
    // 限制得分榜记录数量
    if (highScores_.size() > static_cast<size_t>(maxHighScores_)) {
        highScores_.resize(maxHighScores_);
    }
    
    // 保存得分榜
    saveHighScores();
}

void SceneEnd::handleTextInput(SDL_Event* event) {
    // 获取输入的文本（支持中文）
    std::string inputText = event->text.text;
    
    // 修复 C4267：显式转换 size_t 到 int
    int inputLength = static_cast<int>(inputText.length());
    int currentLength = static_cast<int>(playerName_.length());
    
    // 检查名字长度是否超过限制（15个字符）
    if (currentLength + inputLength <= 15) {
        playerName_ += inputText;
        cursorPosition_ += inputLength;
    }
}

void SceneEnd::handleBackspace() {
    // 处理退格键（支持中文删除）
    if (!playerName_.empty()) {
        // 检查是否为多字节字符（如中文）
        if ((playerName_.back() & 0x80) != 0) {
            // 多字节字符，需要删除多个字节
            size_t bytesToRemove = 1;
            while (bytesToRemove < playerName_.size() && 
                   (playerName_[playerName_.size() - bytesToRemove] & 0xC0) == 0x80) {
                bytesToRemove++;
            }
            playerName_.erase(playerName_.size() - bytesToRemove, bytesToRemove);
        } else {
            // 单字节字符（ASCII字符）
            playerName_.pop_back();
        }
        
        // 修复光标位置更新逻辑
        if (cursorPosition_ > 0) {
            cursorPosition_--;
        }
    }
}

void SceneEnd::switchToTitleScene() {
    // 切换到标题场景
    game_.changeScene(new SceneTitle());
}
