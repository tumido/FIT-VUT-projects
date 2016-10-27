# -*- coding: utf-8 -*-
from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as ec
from selenium.common.exceptions import TimeoutException
from selenium.common.exceptions import StaleElementReferenceException
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities
import unittest


class FreeIpaLoginPage(unittest.TestCase):
    """
    Testsuite for FreeIPA's login page covering the basic login functionality
    """

    def setUp(self):
        self.driver = webdriver.Remote(
            command_executor='http://mys01.fit.vutbr.cz:4444/wd/hub',
            desired_capabilities=DesiredCapabilities.FIREFOX)
        # self.driver = webdriver.Firefox()
        self.driver.implicitly_wait(30)
        self.base_url = "https://ipa.demo1.freeipa.org/ipa/ui/"
        self.verificationErrors = []
        self.accept_next_alert = True

    def user_login_success(self, username, password, role):
        driver = self.driver
        driver.get(self.base_url)

        # log in
        driver.find_element_by_id("username1").send_keys(username)
        driver.find_element_by_id("password2").send_keys(password)
        driver.find_element_by_xpath("//button[@name='login']").click()

        # wait for new page to load
        container = driver.find_element_by_id("container")
        WebDriverWait(driver, 10).until(ec.visibility_of(container))

        # check the user
        role_to_check = driver.find_element_by_class_name("loggedinas").text
        self.assertEqual(role_to_check, role)

        # try to log out
        driver.find_element_by_css_selector("span.loggedinas").click()
        driver.find_element_by_link_text("Logout").click()

    def user_login_failed(self, username, password):
        driver = self.driver
        driver.get(self.base_url)

        # log in
        driver.find_element_by_id("username1").send_keys(username)
        driver.find_element_by_id("password2").send_keys(password)
        driver.find_element_by_xpath("//button[@name='login']").click()

        # check if the allert is shown
        label = driver.find_element_by_xpath("//label[@name='validation']")
        try:
            WebDriverWait(driver, 5).until(ec.visibility_of(label))
        except (TimeoutException, StaleElementReferenceException):
            self.fail("No alert has been shown while using wrong credentials")

    def tearDown(self):
        self.driver.quit()
        self.assertEqual([], self.verificationErrors)

    def test_admin_fail_password_lowercase(self):
        """
        Logging Admin with wrong password.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("admin", "secret123")

    def test_admin_fail_password_uppercase(self):
        """
        Logging Admin with wrong password.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("admin", "SECRET123")

    def test_admin_fail_password_missing(self):
        """
        Logging Admin without password.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("admin", "")

    def test_admin_fail_password_too_long(self):
        """
        Logging Admin with wrong password.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("admin", "x" * 1000)

    def test_admin_fail_basic_sql_inject(self):
        """
        Logging Admin with sql basic injection instead of username.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("' OR '1'='1'", "Secret123")

    def test_admin_fail_username_uppercase(self):
        """
        Logging Admin with wrong username.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("ADMIN", "Secret123")

    def test_admin_fail_username_capitalize(self):
        """
        Logging Admin with wrong username.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("Admin", "Secret123")

    def test_admin_fail_username_missing(self):
        """
        Logging Admin without username.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("", "Secret123")

    def test_admin_fail_credentials_missing(self):
        """
        Logging Admin without credentials.

        Should show alert and fail to log user in.
        """
        self.user_login_failed("", "")

    def test_admin_success(self):
        """
        Test successfull login for Admin account
        """
        self.user_login_success("admin", "Secret123", "Administrator")

    def test_helpdesk_success(self):
        """
        Test successfull login for Helpdesk account
        """
        self.user_login_success("helpdesk", "Secret123", "Test Helpdesk")

    def test_employee_success(self):
        """
        Test successfull login for Employee account
        """
        self.user_login_success("employee", "Secret123", "Test Employee")

    def test_manager_success(self):
        """
        Test successfull login for Manager account
        """
        self.user_login_success("manager", "Secret123", "Test Manager")

    def test_config_links_for_cert(self):
        """
        Kerberos certificate location and availability test

        Collect the links for Kerberos certificate in FreeIPA config guide,
        check if all of them points to the same file and hit the certificate
        url to ensure the file exists.
        """
        driver = self.driver

        cert_loc = list()

        # get first cert location (Firefox config page)
        driver.get(self.base_url)
        driver.find_element_by_link_text("configured").click()
        driver.find_element_by_link_text("Firefox configuration page").click()
        url = driver.find_element_by_id("ca-link").get_attribute('href')
        cert_loc.append(url)

        # manual configuration page cert links
        driver.get(self.base_url)
        driver.find_element_by_link_text("configured").click()
        driver.find_element_by_link_text("manual configuration page").click()

        links = driver.find_elements_by_link_text("CA certificate")
        for link in links:
            cert_loc.append(link.get_attribute('href'))

        # if more than one link is found, check if all of them point to the
        # same location
        if len(cert_loc) > 1:
            self.assertTrue(len(set(cert_loc)) == 1)

        # Try to download the certificate
        driver.get(cert_loc[0])

    def test_access_config_guide(self):
        """
        Test if all sites of the Kerberos config guide are accessible
        """
        driver = self.driver
        driver.get(self.base_url)

        # step by step click on each link and walk through the guide
        # use url (previous and current) to monitor if we're moving and hit
        # some element on the page to see if there is actually a content

        # Config guide home
        previous_url = driver.current_url
        driver.find_element_by_link_text("configured").click()
        heading = driver.find_element_by_tag_name('h1')

        self.assertNotEqual(previous_url, driver.current_url)
        self.assertEqual(heading.text,
                         "Unable to verify your Kerberos credentials")

        # Firefox config
        previous_url = guide_url = driver.current_url
        driver.find_element_by_link_text("Firefox configuration page").click()
        heading = driver.find_element_by_tag_name('h1')

        self.assertNotEqual(previous_url, driver.current_url)
        self.assertEqual(heading.text, "Firefox configuration")

        # Check if return to the Web UI works
        previous_url = driver.current_url
        driver.find_element_by_link_text("Return to Web UI").click()
        base_url = "{0}{1}".format(self.base_url, "index.html")

        self.assertNotEqual(previous_url, driver.current_url)
        self.assertEqual(driver.current_url, base_url)

        # Manual setup guide
        driver.get(guide_url)
        previous_url = driver.current_url
        driver.find_element_by_link_text("manual configuration page").click()
        heading = driver.find_element_by_tag_name('h1')

        self.assertNotEqual(previous_url, driver.current_url)
        self.assertEqual(heading.text, "Browser Kerberos Setup")

if __name__ == "__main__":
    unittest.main()
