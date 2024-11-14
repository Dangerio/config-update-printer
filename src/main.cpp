#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/dynamic_config/client/component.hpp>
#include <userver/dynamic_config/source.hpp>
#include <userver/dynamic_config/storage/component.hpp>
#include <userver/dynamic_config/updater/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

namespace {

inline const userver::dynamic_config::Key kConfig{
    "CONFIG_KEY", std::string("static_default")};

class DynamicConfigPrinter final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "dynamic-config-printer";

  DynamicConfigPrinter(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context)
      : userver::components::ComponentBase(config, context),
        config_subscription_(
            context.FindComponent<userver::components::DynamicConfig>()
                .GetSource()
                .UpdateAndListen(this, kName,
                                 &DynamicConfigPrinter::OnConfigUpdate)){};

 private:
  void OnConfigUpdate(const userver::dynamic_config::Snapshot& config) {
    LOG_INFO() << "Config update: " << config[kConfig];
  }

  userver::concurrent::AsyncEventSubscriberScope config_subscription_;
};

}  // namespace

int main(int argc, char* argv[]) {
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::components::DynamicConfigClient>()
          .Append<userver::components::DynamicConfigClientUpdater>()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::components::HttpClient>()
          .Append<userver::clients::dns::Component>()
          .Append<userver::server::handlers::TestsControl>()
          .Append<DynamicConfigPrinter>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}
