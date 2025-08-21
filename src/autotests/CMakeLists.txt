# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2024-2025 Louis Schul <schul9louis@gmail.com>

ecm_add_test(./parser/extendedSyntaxTest.cpp
    TEST_NAME extendedSyntax
    LINK_LIBRARIES klevernotes_static Qt::Test
    NAME_PREFIX "klevernotes-"
)

ecm_add_test(./parser/noteLinkingTest.cpp
    TEST_NAME noteLinking
    LINK_LIBRARIES klevernotes_static Qt::Test
    NAME_PREFIX "klevernotes-"
)

ecm_add_test(./parser/emojiTest.cpp
    TEST_NAME emoji
    LINK_LIBRARIES klevernotes_static Qt::Test
    NAME_PREFIX "klevernotes-"
)
