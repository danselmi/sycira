#include <zip.h>
#include "CreatMaximaSession.h"

int createMaximaSession(const std::string &sessionName, const std::string &circuitFileName, const std::string &data)
{
    if(checkSessionExists(sessionName))
    {
        createKicad2SyciraMaximaFile(circuitFileName, data);
    }
    else
    {
        createKicad2SyciraMaximaFile(circuitFileName, data);
        createMinimalSession(sessionName, circuitFileName);
    }

    return 0;
}

bool checkSessionExists(const std::string & sessionName)
{

    if (FILE *file = fopen(sessionName.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    else
        return false;
}

int createKicad2SyciraMaximaFile(const std::string &circuitFileName, const std::string &data)
{
    std::ofstream outfile(circuitFileName);
    if (outfile.is_open())
    {
        outfile << data;
        outfile.close();
    }
    else
    {
        std::cerr << "failed to open \"" << circuitFileName << "\" for writing circuit\n";
        return -1;
    }

    return 0;
}

int createMinimalSession(const std::string &sessionName, const std::string &circuitFileName)
{
    zip_t *zip = zip_open(sessionName.c_str(), ZIP_CREATE | ZIP_EXCL, NULL);
            //ZIP_TRUNCATE : If archive exists, ignore its current contents. In other words, handle it the same way as an empty archive.
    if(!zip)
        return -1;


    char mimetype_data[] = "text/x-wxmathml";
    zip_source_t *source1 = zip_source_buffer_create(mimetype_data, sizeof(mimetype_data)/sizeof(mimetype_data[0])-1, 0, NULL);
    zip_uint64_t index = zip_file_add(zip, "mimetype", source1, ZIP_FL_ENC_UTF_8);
    if (index < 0)
    {
        zip_source_free(source1);
        return -3;
    }
    else
        zip_set_file_compression(zip, index, ZIP_CM_STORE, 0);

    char format_txt_data[] = "\n\nThis file contains a wxMaxima session in the .wxmx format.\n"
    ".wxmx files are .xml-based files contained in a .zip container like .odt\n"
    "or .docx files. After changing their name to end in .zip the .xml and\n"
    "eventual bitmap files inside them can be extracted using any .zip file\n"
    "viewer.\n"
    "The reason why part of a .wxmx file still might still seem to make sense in a\n"
    "ordinary text viewer is that the text portion of .wxmx by default\n"
    "isn't compressed: The text is typically small and compressing it would\n"
    "mean that changing a single character would (with a high probability) change\n"
    "big parts of the  whole contents of the compressed .zip archive.\n"
    "Even if version control tools like git and svn that remember all changes\n"
    "that were ever made to a file can handle binary files compression would\n"
    "make the changed part of the file bigger and therefore seriously reduce\n"
    "the efficiency of version control\n\n"
    "wxMaxima can be downloaded from https://github.com/wxMaxima-developers/wxmaxima.\n"
    "It also is part of the windows installer for maxima\n"
    "(https://wxmaxima-developers.github.io/wxmaxima/).\n\n"
    "If a .wxmx file is broken but the content.xml portion of the file can still be\n"
    "viewed using an text editor just save the xml's text as \"content.xml\"\n"
    "and try to open it using a recent version of wxMaxima.\n"
    "If it is valid XML (the XML header is intact, all opened tags are closed again,\n"
    "the text is saved with the text encoding \"UTF8 without BOM\" and the few\n"
    "special characters XML requires this for are properly escaped)\n"
    "chances are high that wxMaxima will be able to recover all code and text\n"
    "from the XML file.\n\n";

    zip_source_t *source2 = zip_source_buffer_create(format_txt_data, sizeof(format_txt_data)/sizeof(format_txt_data[0])-1, 0, NULL);
    index = zip_file_add(zip, "format.txt", source2, ZIP_FL_ENC_UTF_8);
    if (index < 0)
    {
        zip_source_free(source2);
        return -4;
    }
    else
        zip_set_file_compression(zip, index, ZIP_CM_STORE, 0);

//"<!--   Created using wxMaxima 19.07.0   -->\n"

    std::string xml_str =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n"
    "<!--   Created using sycira conv 100   -->\n"
    "<!--https://wxMaxima-developers.github.io/wxmaxima/-->\n\n"
    "<wxMaximaDocument version=\"1.5\" zoom=\"100\" activecell=\"1\">\n\n"
    "<cell type=\"code\">\n"
    "<input>\n"
    "<editor type=\"input\">\n"
    "<line>load(&quot;sycira.mac&quot;);</line>\n"
    "</editor>\n"
    "</input>\n"
    "</cell>\n\n"
    "<cell type=\"code\">\n"
    "<input>\n"
    "<editor type=\"input\">\n"
    "<line>load(&quot;" + circuitFileName + "&quot;);</line>\n"
    "</editor>\n"
    "</input>\n"
    "</cell>\n\n"
    "<cell type=\"code\">\n"
    "<input>\n"
    "<editor type=\"input\">\n"
    "<line>sys:sycira(ckt);</line>\n"
    "</editor>\n"
    "</input>\n"
    "</cell>\n\n"
    "<cell type=\"code\">\n"
    "<input>\n"
    "<editor type=\"input\">\n"
    "<line>solve(sys[1],sys[2]);</line>\n"
    "</editor>\n"
    "</input>\n"
    "</cell>\n\n"
    "</wxMaximaDocument>\n\n";

    zip_source_t *source3 = zip_source_buffer_create(xml_str.c_str(), xml_str.length(), 0, NULL);
    index = zip_file_add(zip, "content.xml", source3, ZIP_FL_ENC_UTF_8);
    if (index < 0)
    {
        zip_source_free(source3);
        return -5;
    }
    else
        zip_set_file_compression(zip, index, ZIP_CM_STORE, 0);



    if(zip_close(zip) != 0)
        return -20;


    return 0;
}
