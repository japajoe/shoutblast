#ifndef SHOUTBLAST_DIALOG_HPP
#define SHOUTBLAST_DIALOG_HPP

#include <string>

namespace ShoutBlast
{
    enum class DialogResult
    {
        None,
        OK,
        Cancel
    };

    class Dialog
    {
    public:
        virtual DialogResult ShowDialog() = 0;
        std::string GetFilePath() const;
        void SetInitialDirectory(const std::string &directoryPath);
        std::string GetInitialDirectory() const;
        void SetTitle(const std::string &title);
        std::string GetTitle() const;
    protected:
        std::string filePath;
        std::string initialDirectory;
        std::string title;
    };

    class OpenFileDialog : public Dialog
    {
    public:
        OpenFileDialog();
        DialogResult ShowDialog() override;
    };

    class SaveFileDialog : public Dialog
    {
    public:
        SaveFileDialog();
        DialogResult ShowDialog() override;

    };
}

#endif