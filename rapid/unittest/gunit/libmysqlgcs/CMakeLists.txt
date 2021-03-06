# Copyright (c) 2016, 2017 Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

IF(NOT GMOCK_FOUND OR DISABLE_SHARED)
  RETURN()
ENDIF()

###
### Includes
###

INCLUDE_DIRECTORIES(
  SYSTEM
  ${GMOCK_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/rapid/plugin/group_replication/libmysqlgcs/include
  ${CMAKE_BINARY_DIR}/rapid/plugin/group_replication/libmysqlgcs/include
  ${CMAKE_BINARY_DIR}/rapid/plugin/group_replication/libmysqlgcs/xdr_gen
  ${CMAKE_SOURCE_DIR}/rapid/plugin/group_replication/libmysqlgcs/src/bindings/xcom
  ${CMAKE_SOURCE_DIR}/rapid/plugin/group_replication/libmysqlgcs/src/bindings/xcom/xcom)

IF (WIN32)
  SET (XCOM_BASEDIR
       ${CMAKE_SOURCE_DIR}/rapid/plugin/group_replication/libmysqlgcs/src/bindings/xcom/xcom)
  INCLUDE_DIRECTORIES(${XCOM_BASEDIR}/windeps/include
                      ${XCOM_BASEDIR}/windeps/sunrpc
                      ${XCOM_BASEDIR}/windeps/sunrpc/rpc)
ENDIF()

###
### Tests
###

SET(GCS_INTERFACE_TESTS
    interface/gcs_message)

SET(GCS_XCOM_TESTS
    xcom/gcs_logging
    xcom/gcs_parameters
    xcom/gcs_xcom_notification
    xcom/gcs_xcom_utils
    xcom/gcs_msg_stages
    xcom/gcs_whitelist
    xcom/gcs_xcom_group_management
    xcom/gcs_xcom_state_exchange
    xcom/gcs_packet
    xcom/gcs_xcom_communication_interface
    xcom/gcs_xcom_interface_unit
    xcom/gcs_xcom_statistics_interface
   )

###
### Configure the tests and add target
###

FOREACH(tpath ${GCS_INTERFACE_TESTS} ${GCS_XCOM_TESTS})
  GET_FILENAME_COMPONENT(test ${tpath} NAME)
  ADD_EXECUTABLE(${test}-t ${tpath}-t.cc)
  TARGET_LINK_LIBRARIES(${test}-t gunit_small ${GCS_LIBRARY} ${LZ4_LIBRARY})
  ADD_TEST(${test} ${test}-t)
  SET(GCS_UNIT_TESTS "${GCS_UNIT_TESTS}" "${test}-t")
ENDFOREACH()

ADD_CUSTOM_TARGET(libmysqlgcs-tests
                  DEPENDS
                  ${GCS_LIBRARY}
                  ${LZ4_LIBRARY}
                  ${GCS_UNIT_TESTS})
