target_link_libraries(ClothSimulation glad glfw3dll)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
   target_link_libraries(ClothSimulation FreeImaged)
else()
   target_link_libraries(ClothSimulation FreeImage)
endif()