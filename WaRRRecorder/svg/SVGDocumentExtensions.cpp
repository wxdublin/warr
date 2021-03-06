/*
 * Copyright (C) 2006 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"

#if ENABLE(SVG)
#include "SVGDocumentExtensions.h"

#include "Console.h"
#include "DOMWindow.h"
#include "Document.h"
#include "EventListener.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "Page.h"
#include "SMILTimeContainer.h"
#include "SVGSMILElement.h"
#include "SVGSVGElement.h"
#include "ScriptController.h"
#include "ScriptableDocumentParser.h"
#include <wtf/text/AtomicString.h>

namespace WebCore {

SVGDocumentExtensions::SVGDocumentExtensions(Document* document)
    : m_document(document)
    , m_resourcesCache(adoptPtr(new SVGResourcesCache))
{
}

SVGDocumentExtensions::~SVGDocumentExtensions()
{
    deleteAllValues(m_pendingResources);
}

void SVGDocumentExtensions::addTimeContainer(SVGSVGElement* element)
{
    m_timeContainers.add(element);
}

void SVGDocumentExtensions::removeTimeContainer(SVGSVGElement* element)
{
    m_timeContainers.remove(element);
}

void SVGDocumentExtensions::addResource(const AtomicString& id, RenderSVGResourceContainer* resource)
{
    ASSERT(resource);

    if (id.isEmpty())
        return;

    // Replaces resource if already present, to handle potential id changes
    m_resources.set(id, resource);
}

void SVGDocumentExtensions::removeResource(const AtomicString& id)
{
    if (id.isEmpty() || !m_resources.contains(id))
        return;

    m_resources.remove(id);
}

RenderSVGResourceContainer* SVGDocumentExtensions::resourceById(const AtomicString& id) const
{
    if (id.isEmpty())
        return 0;

    return m_resources.get(id);
}

void SVGDocumentExtensions::startAnimations()
{
    // FIXME: Eventually every "Time Container" will need a way to latch on to some global timer
    // starting animations for a document will do this "latching"
#if ENABLE(SVG_ANIMATION)    
    HashSet<SVGSVGElement*>::iterator end = m_timeContainers.end();
    for (HashSet<SVGSVGElement*>::iterator itr = m_timeContainers.begin(); itr != end; ++itr)
        (*itr)->timeContainer()->begin();
#endif
}
    
void SVGDocumentExtensions::pauseAnimations()
{
    HashSet<SVGSVGElement*>::iterator end = m_timeContainers.end();
    for (HashSet<SVGSVGElement*>::iterator itr = m_timeContainers.begin(); itr != end; ++itr)
        (*itr)->pauseAnimations();
}

void SVGDocumentExtensions::unpauseAnimations()
{
    HashSet<SVGSVGElement*>::iterator end = m_timeContainers.end();
    for (HashSet<SVGSVGElement*>::iterator itr = m_timeContainers.begin(); itr != end; ++itr)
        (*itr)->unpauseAnimations();
}

bool SVGDocumentExtensions::sampleAnimationAtTime(const String& elementId, SVGSMILElement* element, double time)
{
#if !ENABLE(SVG_ANIMATION)
    UNUSED_PARAM(elementId);
    UNUSED_PARAM(element);
    UNUSED_PARAM(time);
    return false;
#else
    ASSERT(element);
    SMILTimeContainer* container = element->timeContainer();
    if (!container || container->isPaused())
        return false;

    container->sampleAnimationAtTime(elementId, time);
    return true;
#endif
}

// FIXME: Callers should probably use ScriptController::eventHandlerLineNumber()
static int parserLineNumber(Document* document)
{
    ScriptableDocumentParser* parser = document->scriptableDocumentParser();
    if (!parser)
        return 1;
    return parser->lineNumber();
}

static void reportMessage(Document* document, MessageLevel level, const String& message)
{
    if (Frame* frame = document->frame())
        frame->domWindow()->console()->addMessage(JSMessageSource, LogMessageType, level, message, parserLineNumber(document), String());
}

void SVGDocumentExtensions::reportWarning(const String& message)
{
    reportMessage(m_document, WarningMessageLevel, "Warning: " + message);
}

void SVGDocumentExtensions::reportError(const String& message)
{
    reportMessage(m_document, ErrorMessageLevel, "Error: " + message);
}

void SVGDocumentExtensions::addPendingResource(const AtomicString& id, PassRefPtr<SVGStyledElement> obj)
{
    ASSERT(obj);

    if (id.isEmpty())
        return;

    if (m_pendingResources.contains(id))
        m_pendingResources.get(id)->add(obj);
    else {
        SVGPendingElements* set = new SVGPendingElements;
        set->add(obj);

        m_pendingResources.add(id, set);
    }
}

bool SVGDocumentExtensions::isPendingResource(const AtomicString& id) const
{
    if (id.isEmpty())
        return false;

    return m_pendingResources.contains(id);
}

PassOwnPtr<HashSet<RefPtr<SVGStyledElement> > > SVGDocumentExtensions::removePendingResource(const AtomicString& id)
{
    ASSERT(m_pendingResources.contains(id));

    OwnPtr<SVGPendingElements> set(m_pendingResources.get(id));
    m_pendingResources.remove(id);
    return set.release();
}

}

#endif
