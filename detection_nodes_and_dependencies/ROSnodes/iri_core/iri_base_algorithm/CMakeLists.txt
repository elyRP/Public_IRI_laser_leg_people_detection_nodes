cmake_minimum_required(VERSION 3.5)
project(iri_base_algorithm)




# Encontrar dependencias
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(diagnostic_updater REQUIRED)
# find_package(dynamic_reconfigure REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

# Verifica si Boost fue encontrado
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif()

# Incluir directorios
include_directories(
  include
  ${Boost_INCLUDE_DIRS}
)

# Declarar una biblioteca C++

add_library(${PROJECT_NAME} include)
# Especifica las propiedades de la biblioteca INTERFACE
target_include_directories(${PROJECT_NAME}
  PRIVATE include
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  rclcpp_components
  diagnostic_updater
  # dynamic_reconfigure
  Boost
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

#add_library(${PROJECT_NAME} INCLUDE ${PROJECT_NAME}/iri_base_algorithm.h)
#target_link_libraries(iri_base_algorithm ${PROJECT_NAME})
#target_include_directories(${PROJECT_NAME} PRIVATE include)


# Exportar dependencias
ament_export_dependencies(rclcpp rclcpp_components diagnostic_updater  Boost)
ament_export_include_directories(include)  # dynamic_reconfigure
ament_export_libraries(${PROJECT_NAME})

# Instalación
install(TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  INCLUDES DESTINATION include
)

install(DIRECTORY include/
  DESTINATION include/
)

ament_package()

