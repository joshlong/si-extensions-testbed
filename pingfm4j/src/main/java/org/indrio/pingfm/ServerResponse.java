/*
 * Copyright 2010 the original author or authors
 *
 *     Licensed under the Apache License, Version 2.0 (the "License");
 *     you may not use this file except in compliance with the License.
 *     You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */
package org.indrio.pingfm;

import org.indrio.pingfm.beans.Message;
import org.indrio.pingfm.beans.Service;
import org.indrio.pingfm.beans.Trigger;

import java.io.Serializable;
import java.util.List;

public class ServerResponse
        implements Serializable {
    private static final long serialVersionUID = -3758085965897291883L;
    private final String status_;
    private final String transactionId_;
    private final String method_;
    private final String errorMessage_;
    private List<Message> messages_;
    private List<Service> services_;
    private List<Trigger> triggers_;

    public ServerResponse(String _status, String _transactionId, String _method, String _errorMessage) {
        super();
        this.status_ = _status;
        this.transactionId_ = _transactionId;
        this.method_ = _method;
        this.errorMessage_ = _errorMessage;
    }

    public String getStatus() {
        return status_;
    }

    public String getTransactionId() {
        return transactionId_;
    }

    public String getMethod() {
        return method_;
    }

    public String getErrorMessage() {
        return errorMessage_;
    }

    public List<Message> getMessages() {
        return messages_;
    }

    public List<Service> getService() {
        return services_;
    }

    public List<Trigger> getTrigger() {
        return triggers_;
    }

    public void setMessages(List<Message> _messages) {
        this.messages_ = _messages;
    }

    public void setService(List<Service> _services) {
        this.services_ = _services;
    }

    public void setTrigger(List<Trigger> _triggers) {
        this.triggers_ = _triggers;
    }

    public void addMessage(Message _message) {
        this.messages_.add(_message);
    }

    public void addService(Service _service) {
        this.services_.add(_service);
    }

    public void addTrigger(Trigger _trigger) {
        this.triggers_.add(_trigger);
    }
}
