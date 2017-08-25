import sys
import os

from mako.template import Template

import cwap


ROOT = os.path.dirname(os.path.abspath(__file__))


if __name__ == '__main__':
    project = cwap.Project(sys.argv)

    if not os.path.exists(project.name):
        os.mkdirs(project.name)
    
    with open(os.path.join(project.name, '__init__.py'), 'w') as init:
        with open(os.path.join(ROOT, '__init__.py.mako'), 'r') as init_template_file:
            template = init_template_file.read()
        init.write(Template(template).render(project=project))

    # with open(os.path.join(project.name, 'wrapped.hpp'), 'w') as header:
    #     with open(os.path.join(ROOT, 'wrapped.hpp.mako'), 'r') as header_template_file:
    #         template = header_template_file.read()
    #     header.write(Template(template).render(project=project))

    with open(os.path.join(project.name, 'wrapped.cpp'), 'w') as wrapped:
        with open(os.path.join(ROOT, 'wrapped.cpp.mako'), 'r') as wrapped_template_file:
            template = wrapped_template_file.read()
        wrapped.write(Template(template).render(project=project))

