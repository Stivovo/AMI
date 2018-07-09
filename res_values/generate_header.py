import xml.etree.ElementTree as ET


TEMPLATE = """\
#ifndef AMI_RES_TABLE_H_
#define AMI_RES_TABLE_H_
#include <map>


// THIS FILE IS AUTO-GENERATED

namespace AMI {{
namespace details {{

using res_map_t = std::map<uint32_t, const char*>;

static const res_map_t res_map {{
{entries:s}
}};



}} // Namespace details
}} // Namespace AMI


#endif

"""

TEMPLATE_ENTRY = """    {{ 0x{resval:04x}, "{name}" }},"""

tree = ET.parse("./android_8.1.0_public.xml")
root = tree.getroot()


element_formated = ""

for element in root.iter("public"):
    id    = int(element.attrib["id"], 16)
    name  = element.attrib["name"]
    etype = element.attrib["type"]
    element_formated += "\n"
    element_formated += TEMPLATE_ENTRY.format(resval=id, name=name);


file_formated = TEMPLATE.format(entries=element_formated)
print(file_formated)



