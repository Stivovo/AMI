if(__add_ami_options)
	return()
endif()
set(__add_ami_options ON)

option(AMI_USE_CCACHE      "Use ccache to speed up compilation" ON)
option(AMI_PYTHON_BINDINGS "Enable Python Bindings"             OFF)

