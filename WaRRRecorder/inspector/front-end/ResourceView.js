/*
 * Copyright (C) 2007, 2008 Apple Inc.  All rights reserved.
 * Copyright (C) IBM Corp. 2009  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

WebInspector.ResourceView = function(resource)
{
    WebInspector.View.call(this);
    this.element.addStyleClass("resource-view");
    this.resource = resource;
}

WebInspector.ResourceView.prototype = {
    hasContent: function()
    {
        return false;
    }
}

WebInspector.ResourceView.prototype.__proto__ = WebInspector.View.prototype;

WebInspector.ResourceView.createResourceView = function(resource)
{
    switch (resource.category) {
    case WebInspector.resourceCategories.documents:
    case WebInspector.resourceCategories.stylesheets:
    case WebInspector.resourceCategories.scripts:
    case WebInspector.resourceCategories.xhr:
        return new WebInspector.SourceView(resource);
    case WebInspector.resourceCategories.images:
        return new WebInspector.ImageView(resource);
    case WebInspector.resourceCategories.fonts:
        return new WebInspector.FontView(resource);
    default:
        return new WebInspector.ResourceView(resource);
    }
}

WebInspector.ResourceView.resourceViewTypeMatchesResource = function(resource)
{
    var resourceView = resource._resourcesView;
    switch (resource.category) {
    case WebInspector.resourceCategories.documents:
    case WebInspector.resourceCategories.stylesheets:
    case WebInspector.resourceCategories.scripts:
    case WebInspector.resourceCategories.xhr:
        return resourceView.__proto__ === WebInspector.SourceView.prototype;
    case WebInspector.resourceCategories.images:
        return resourceView.__proto__ === WebInspector.ImageView.prototype;
    case WebInspector.resourceCategories.fonts:
        return resourceView.__proto__ === WebInspector.FontView.prototype;
    default:
        return resourceView.__proto__ === WebInspector.ResourceView.prototype;
    }
}

WebInspector.ResourceView.resourceViewForResource = function(resource)
{
    if (!resource)
        return null;
    if (!resource._resourcesView)
        resource._resourcesView = WebInspector.ResourceView.createResourceView(resource);
    return resource._resourcesView;
}

WebInspector.ResourceView.recreateResourceView = function(resource)
{
    var newView = WebInspector.ResourceView.createResourceView(resource);

    var oldView = resource._resourcesView;
    var oldViewParentNode = oldView.visible ? oldView.element.parentNode : null;
    var scrollTop = oldView.scrollTop;

    resource._resourcesView.detach();
    delete resource._resourcesView;

    resource._resourcesView = newView;

    if (oldViewParentNode)
        newView.show(oldViewParentNode);
    if (scrollTop)
        newView.scrollTop = scrollTop;

    WebInspector.panels.scripts.viewRecreated(oldView, newView);
    return newView;
}

WebInspector.ResourceView.existingResourceViewForResource = function(resource)
{
    if (!resource)
        return null;
    return resource._resourcesView;
}
