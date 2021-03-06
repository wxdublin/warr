/*
Copyright 2010 WebDriver committers
Copyright 2010 Google Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package org.openqa.selenium.remote.server.handler;

import org.openqa.selenium.Rotatable;
import org.openqa.selenium.remote.Response;
import org.openqa.selenium.remote.server.DriverSessions;
import org.openqa.selenium.remote.server.rest.ResultType;

public class GetScreenOrientation extends WebDriverHandler {
  private volatile Response response;

  public GetScreenOrientation(DriverSessions sessions) {
    super(sessions);
  }

  public ResultType call() throws Exception {
    response = newResponse();
    response.setValue(((Rotatable) unwrap(getDriver())).getOrientation());
    return ResultType.SUCCESS;
  }
  
  public Response getResponse() {
    return response;
  }

  @Override
  public String toString() {
    return "[get screen orientation]";
  }
}
