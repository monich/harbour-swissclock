/*
 * Copyright (C) 2014-2016 Jolla Ltd.
 * Contact: Slava Monich <slava.monich@jolla.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   - Neither the name of Jolla Ltd nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "QuickClockSeconds.h"
#include "ClockDebug.h"

#include <QSGGeometryNode>
#include <QSGSimpleRectNode>

#define SUPER QuickClockLayer

QuickClockSeconds::QuickClockSeconds(QuickClock* aParent) :
    SUPER(aParent)
{
    QTRACE("- created");
}

int
QuickClockSeconds::msecUntilNextUpdate()
{
    return QUICK_CLOCK_MIN_UPDATE_INTERVAL_DISPLAY_ON;
}

QSGNode*
QuickClockSeconds::createNode(
    const QSize& aSize)
{
    QSGNode* node = new QSGNode;
    QSGTransformNode* txNode = new QSGTransformNode;
    node->appendChildNode(txNode);
    renderer()->initSecNode(txNode, aSize, window(), theme());
    return node;
}

void
QuickClockSeconds::updateNode(
    QSGNode* aNode,
    const QSize& aSize,
    const QTime& aTime)
{
    QMatrix4x4 matrix = renderer()->secNodeMatrix(aSize, aTime);
    for (QSGNode* n = aNode->firstChild(); n; n = n->nextSibling()) {
        if (n->type() == QSGNode::TransformNodeType) {
            QSGTransformNode* txNode = (QSGTransformNode*)n;
            txNode->setMatrix(matrix);
        }
    }
    CLOCK_PERFORMANCE_LOG_RECORD;
}
