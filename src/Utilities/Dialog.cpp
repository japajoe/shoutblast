#include "Dialog.hpp"
#include "../../libs/tinyfiledialogs/include/tinyfiledialogs.h"
#include <filesystem>

namespace ShoutBlast
{
    std::string Dialog::GetFilePath() const
    {
        return filePath;
    }

    void Dialog::SetInitialDirectory(const std::string &directoryPath)
    {
        std::error_code error;
        std::filesystem::path absolutePath = std::filesystem::absolute(directoryPath, error);

        if (!error)
            initialDirectory = absolutePath.string();
        else
            initialDirectory = directoryPath;
    }

    std::string Dialog::GetInitialDirectory() const
    {
        return initialDirectory;
    }

    void Dialog::SetTitle(const std::string &title)
    {
        this->title = title;
    }

    std::string Dialog::GetTitle() const
    {
        return title;
    }

    OpenFileDialog::OpenFileDialog()
    {
        title = "Open File";
    }

    DialogResult OpenFileDialog::ShowDialog()
    {
        const char * filterPatterns[1] = { "*" };
        const char * result = tinyfd_openFileDialog(
            title.c_str(),
            initialDirectory.c_str(),
            0,
            NULL,
            NULL,
            0
        );

        if (result)
        {
            filePath = result;
            return DialogResult::OK;
        }

        return DialogResult::Cancel;
    }

    SaveFileDialog::SaveFileDialog()
    {
        title = "Save As";
    }

    DialogResult SaveFileDialog::ShowDialog()
    {
        const char * filterPatterns[1] = { "*" };
        const char * result = tinyfd_saveFileDialog(
            title.c_str(),
            initialDirectory.c_str(),
            0,
            NULL,
            NULL
        );

        if (result)
        {
            filePath = result;
            return DialogResult::OK;
        }

        return DialogResult::Cancel;
    }
}