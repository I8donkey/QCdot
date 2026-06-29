#ifndef CONSOLE_REDIRECTOR_H
#define CONSOLE_REDIRECTOR_H

#include <iostream>
#include <streambuf>
#include <string>
#include <QObject>
#include <QString>

class ConsoleRedirector : public QObject {
    Q_OBJECT
public:
    ConsoleRedirector(QObject* parent = nullptr) : QObject(parent) {
        oldStdout_ = std::cout.rdbuf();
        oldStderr_ = std::cerr.rdbuf();
        
        std::cout.rdbuf(&stdoutBuf_);
        std::cerr.rdbuf(&stderrBuf_);
        
        stdoutBuf_.setCallback([this](const std::string& text) {
            emit outputReceived(QString::fromStdString(text));
        });
        
        stderrBuf_.setCallback([this](const std::string& text) {
            emit outputReceived(QString::fromStdString(text));
        });
    }
    
    ~ConsoleRedirector() {
        std::cout.rdbuf(oldStdout_);
        std::cerr.rdbuf(oldStderr_);
    }
    
signals:
    void outputReceived(const QString& text);
    
private:
    class CallbackBuf : public std::streambuf {
    public:
        using Callback = std::function<void(const std::string&)>;
        
        void setCallback(Callback cb) { callback_ = std::move(cb); }
        
    protected:
        int_type overflow(int_type ch) override {
            if (ch != traits_type::eof()) {
                buffer_ += static_cast<char>(ch);
                if (ch == '\n') {
                    flushBuffer();
                }
            }
            return ch;
        }
        
        std::streamsize xsputn(const char* s, std::streamsize n) override {
            buffer_.append(s, static_cast<size_t>(n));
            size_t pos = 0;
            while ((pos = buffer_.find('\n')) != std::string::npos) {
                std::string line = buffer_.substr(0, pos);
                buffer_ = buffer_.substr(pos + 1);
                if (callback_) callback_(line);
            }
            return n;
        }
        
        int sync() override {
            flushBuffer();
            return 0;
        }
        
    private:
        void flushBuffer() {
            if (!buffer_.empty() && callback_) {
                callback_(buffer_);
                buffer_.clear();
            }
        }
        
        std::string buffer_;
        Callback callback_;
    };
    
    std::streambuf* oldStdout_ = nullptr;
    std::streambuf* oldStderr_ = nullptr;
    CallbackBuf stdoutBuf_;
    CallbackBuf stderrBuf_;
};

#endif // CONSOLE_REDIRECTOR_H
