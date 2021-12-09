include $(TOPDIR)/rules.mk

PKG_NAME:=openvpn-manager
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/openvpn-manager
	CATEGORY:=Base system
	TITLE:=openvpn-manager
	DEPENDS:=+libuci +libjson-c +libubus +libubox +libblobmsg-json
endef

define Package/openvpn-manager/description
	openvpn ubus management system. Uses the openvpn package.
endef

define Package/openvpn-manager/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/openvpn-manager $(1)/usr/bin
endef

$(eval $(call BuildPackage,openvpn-manager))