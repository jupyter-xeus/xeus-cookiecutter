
class TestBake(object):

    def test_default(self, cookies):

        result = cookies.bake()

        assert result.exit_code == 0
        assert result.exception is None

        assert result.project_path.name == "xeus-lua"
        assert result.project_path.is_dir()
