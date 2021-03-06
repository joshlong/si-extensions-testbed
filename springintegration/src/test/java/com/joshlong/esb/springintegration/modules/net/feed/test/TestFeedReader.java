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
package com.joshlong.esb.springintegration.modules.net.feed.test;

import org.junit.Test;

import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.AbstractJUnit4SpringContextTests;


/**
 * A simple demonstration of the feed entry consumption mechanism.
 *
 * @author <a href="mailto:josh@joshlong.com">Josh Long</a>
 */
@ContextConfiguration(locations =  {
    "/net/feed/test_inbound_feeds.xml"}
)
public class TestFeedReader extends AbstractJUnit4SpringContextTests {
    @Test
    public void testItGood() throws Throwable {
        if (System.in.read() <= 0) {
            logger.debug("returning after test");
        }
    }
}
